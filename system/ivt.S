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

_software_interrupt:
    ldr r1, [lr,#-4]
    bic r1, r1, #0xFF000000

    cmp r1, #1
    beq _swi_fork

    cmp r1, #2
    beq _swi_exit

    b handle_software_interrupt


_swi_fork:
    bl _push_usr_ctx

    mov r0, #0x0
    bl _push_r0_usr
    
    bl spawn_thread

    // span_thread returns the child's id in r0
    stmfd sp!, {r0}
    bl _pop_usr_ctx
    // Overwrite r0 with the child id.
    ldmfd sp!, {r0}

    movs pc, lr

_swi_exit:
    // Stop the current thread and switch to the next one in the runqueue.
    bl exit_thread
    // Pop the next thread's context.
    bl _pop_usr_ctx
    movs pc, lr


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

_data_abort:
    stmfd sp!, {r0-r3, r12, lr}
    bl handle_data_abort
    ldmfd sp!, {r0-r3, r12, pc}^
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

