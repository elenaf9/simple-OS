.section .text
.global _irq
_irq:
    // Adjust and save LR of current mode in current stack
    sub lr, lr, #4
    stmfd sp!, {lr}

     
    // Save SPSR and r0 in current stack
    mrs lr, spsr
    stmfd sp!, {r0-r12, lr}

    // Save SP_usr, LR_usr
    stm r4, {r13, r14}^
    
    // Store IRQ stack
    mov r5, sp

    // Switch to sys mode, enable interrupt
    mov r0, #0b11111
    bl _switch_cpu_mode

    bl _cpu_enable_interrupt

    // Use IRQ Stack
    mov sp, r5

    bl handle_irq

    // Switch back to IRQ mode
    mov r0, #0b10010
    bl _switch_cpu_mode
    bl _cpu_disable_interrupt

    // Restore SP_usr, LR_usr
    ldm r4, {r13, r14}^

    ldmfd sp!, {r0-r12, lr}
    msr spsr, lr
    
    // Restore adjusted LR_irq from IRQ stack directly in the PC
    ldmfd sp!, {pc}^
