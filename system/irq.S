.section .text
.global _irq
_irq:
    sub lr, lr, #4

    // Save r0 so we can use it for our operations.
    stmfd sp!, {r0}

    mov r0, lr
    bl _push_usr_ctx
    
    ldmfd sp!, {r0}
    bl _push_r0_usr

    bl handle_irq
    bl end_of_interrupt

    bl _pop_usr_ctx;

    movs pc, lr
