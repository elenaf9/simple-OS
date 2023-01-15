.section .ivt, "ax", %progbits
ivt:
 	ldr	pc, _jt_reset	/* Reset */ 
 	ldr	pc, _jt_undef		/* Undefined Instruction */ 
 	ldr	pc, _jt_swi		/* Software Interrupt */ 
 	ldr	pc, _jt_pref_abort	/* Prefetch Abort */ 
 	ldr	pc, _jt_data_abort	/* Data Abort */ 
 	nop
 	ldr pc, [pc, #-0xF20]	// AIC Interrupt vector table
 	ldr	pc, _jt_fiq	/* Fast Interrupt */ 

_jt_reset:      .word _reset
_jt_undef:	    .word _undefined_instruction
_jt_swi:		.word _software_interrupt
_jt_pref_abort: .word _prefetched_abort
_jt_data_abort:	.word _data_abort
_jt_fiq:	    .word _fiq

.section .text
_reset:
    bl handle_reset
    b _Lend
    
_undefined_instruction:
    stmfd sp!, {r0-r4, r12, lr}
    mov r4, lr
    sub r4, #4 // Instruction that triggered the exception.
    mov r0, r4
    bl handle_undefined_instruction
    ldmfd sp!, {r0-r4, r12, pc}^
    b _Lend

_software_interrupt:
    ldr r1, [lr,#-4]
    bic r1, r1, #0xFF000000

    cmp r1, #1
    beq _swi_fork

    cmp r1, #2
    beq _swi_exit

    b handle_software_interrupt


_swi_fork:

    mov lr, r0
    // Load usr_sp into r0 and save context on it.
    stmfd sp!, {r13}^
    ldmfd sp!, {r0}

    // Store context
    stmfd r0!, {r14}^
    stmfd r0!, {r1-r12, r14}
    // r1 will be loaded into r0
    mov r1, #0x0
    stmfd r0!, {r1}

    mrs r1, spsr
    stmfd r0!, {r1}

    stmfd sp!, {r0}
    ldmfd sp!, {r13}^
    
    bl spawn_thread
    mov r2, r0

    // Load usr_sp into r0 and restore context from it.
    stmfd sp!, {r13}^
    ldmfd sp!, {r0}

    // Restore spsr
    ldmfd r0!, {r1}
    msr spsr, r1

    // Restore context
    ldmfd r0!, {r1} // Pop old r0, but don't use it.
    // Temporary save r2, which will be later loaded into r0.
    stmfd sp!, {r2}

    ldmfd r0!, {r1-r12, r14}
    ldmfd r0!, {r14}^

    stmfd sp!, {r0}
    ldmfd sp!, {r13}^

    // Finally, restore r0.
    ldmfd sp!, {r0}

    movs pc, lr

_swi_exit:
    b exit_thread

_data_abort:
    stmfd sp!, {r0-r3, r12, lr}
    bl handle_data_abort
    // ldmfd sp!, {r0-r3, r12, pc}^
    b _Lend

_prefetched_abort:
    stmfd sp!, {r0-r3, r12, lr}
    bl handle_data_abort
    // ldmfd sp!, {r0-r3, r12, pc}^
    b _Lend
    
_fiq:
    stmfd sp!, {r0-r3, r12, lr}
    bl handle_fiq
    ldmfd sp!, {r0-r3, r12, pc}^

_Lend:
b _Lend

