.section .ivt, "ax", %progbits
ivt:
 	ldr	pc, jt_reset	/* Reset */ 
 	ldr	pc, jt_undef		/* Undefined Instruction */ 
 	ldr	pc, jt_swi		/* Software Interrupt */ 
 	ldr	pc, jt_pref_abort	/* Prefetch Abort */ 
 	ldr	pc, jt_data_abort	/* Data Abort */ 
 	nop	/* Reserved */ 
 	ldr	pc, jt_irq	/* Interrupt */ 
 	ldr	pc, jt_fiq	/* Fast Interrupt */ 

jt_reset:       .word reset
jt_undef:	    .word undefined_instruction
jt_swi:		    .word software_interrupt
jt_pref_abort:	.word prefetched_abort
jt_data_abort:	.word data_abort
jt_irq:	        .word irq
jt_fiq:	        .word fiq

.section .text
reset:
    bl handle_reset
    b .Lend
    
undefined_instruction:
    stmfd sp!, {r0-r4, r12, lr}
    mov r4, lr
    sub r4, #4 // Instruction that triggered the exception.
    mov r0, r4
    bl handle_undefined_instruction
    // ldmfd sp!, {r0-r4, r12, pc}^
    b .Lend

software_interrupt:
    stmfd sp!, {lr}
    bl handle_software_interrupt
    ldmfd sp!, {pc}^

data_abort:
    stmfd sp!, {r0-r3, r12, lr}
    bl handle_data_abort
    // ldmfd sp!, {r0-r4, r12, pc}^
    b .Lend

prefetched_abort:
    stmfd sp!, {r0-r3, r12, lr}
    bl handle_data_abort
    // ldmfd sp!, {r0-r4, r12, pc}^
    b .Lend

irq:
    sub lr, lr, #4
    stmfd sp!, {r0-r3, r12, lr}
    mov r0, lr
    bl handle_irq
    ldmfd sp!, {r0-r3, r12, pc}^

fiq:
    stmfd sp!, {r0-r3, r12, lr}
    bl handle_fiq
    ldmfd sp!, {r0-r3, r12, pc}^

.Lend:
b .Lend


    







