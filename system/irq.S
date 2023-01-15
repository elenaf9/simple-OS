.section .text
.global _irq
.global _end_irq
_irq:
    sub lr, lr, #4
    // Save r0 so we can use it for our operations.
    stmfd sp!, {r0}

    // Load usr_sp into r0 and save context on it.
    stmfd sp!, {r13}^
    ldmfd sp!, {r0}

    // Store context
    stmfd r0!, {r14}^
    stmfd r0!, {r1-r12, r14}
    // r1 is the r0 we saved above before using r0 for the usr_sp.
    ldmfd sp!, {r1}
    stmfd r0!, {r1}

    mrs r1, spsr
    stmfd r0!, {r1}

    stmfd sp!, {r0}
    ldmfd sp!, {r13}^

    bl handle_irq
    bl end_of_interrupt

    // Load usr_sp into r0 and restore context from it.
    stmfd sp!, {r13}^
    ldmfd sp!, {r0}

    // Restore spsr
    ldmfd r0!, {r1}
    msr spsr, r1

    // Restore context
    // Temporary save r1, which will be later loaded into r0.
    ldmfd r0!, {r1}
    stmfd sp!, {r1}

    ldmfd r0!, {r1-r12, r14}
    ldmfd r0!, {r14}^

    stmfd sp!, {r0}
    ldmfd sp!, {r13}^

    // Finally, restore r0.
    ldmfd sp!, {r0}

    movs pc, lr
