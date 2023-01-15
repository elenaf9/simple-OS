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
    mov lr, r2
    stmfd sp!, {lr}
    
    bl spawn_thread
    ldmfd sp!, {pc}^

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
