.section .text
.global _irq
.global _end_irq
_irq:
    // Adjust and save LR of current mode in current stack
    sub lr, lr, #4
    stmfd sp!, {lr}

    stmfd sp!, {r0-r3, r12}

    bl handle_irq

_end_irq:

    ldmfd sp!, {r0-r3, r12}

    // Restore adjusted LR_irq from IRQ stack directly in the PC
    ldmfd sp!, {pc}^
