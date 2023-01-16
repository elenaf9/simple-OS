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

    cmp r1, #3
    beq _swi_sleep

    cmp r1, #4
    beq _swi_read

    cmp r1, #5
    beq _swi_write

_swi_fork:
    // R0 was already set to the LR in the `fork` sys_call
    bl _push_usr_ctx
    // r0 is the thread PID.
    // For the child we set it to 0 here,
    // which will be popped into r0 when the context is
    // restored in the irq after the thread switch.
    // For the parent we overwrite it with the child's id
    // after the `spawn_thread` call below.
    mov r0, #0x0
    bl _push_r0_usr

    bl spawn_thread

    // spawn_thread returns the child's id in r0
    stmfd sp!, {r0}
    bl _pop_usr_ctx
    // Overwrite r0 with the child id.
    ldmfd sp!, {r0}

    movs pc, lr

_swi_exit:
    // Don't store current context, but restore new context after the 
    // `exit_thread` call since the thread is switched there.
    bl exit_thread
    bl _pop_usr_ctx
    movs pc, lr

_swi_sleep:
    // r0 are the ms that the thread should sleep.
    stmfd sp!, {r0}

    mov r0, lr
    bl _push_usr_ctx

    eor r0, r0 
    bl _push_r0_usr

    ldmfd sp!, {r0}

    bl thread_sleep

    bl _pop_usr_ctx;
    movs pc, lr

_swi_read:
    mov r0, lr
    bl _push_usr_ctx

    eor r0, r0
    bl _push_r0_usr

    bl thread_wait_char

    bl _pop_usr_ctx;
    movs pc, lr

_swi_write:
    stmfd sp!, {lr}
    bl write_char
    ldmfd sp!, {pc}^

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

