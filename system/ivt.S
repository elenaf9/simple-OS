.section .ivt, "ax", %progbits
ivt:
    b reset
    b undefined_instruction
    b software_interrupt
    b prefetched_abort
    b data_abort
    nop
    b irq
    b fiq


reset:
    bl handle_reset
    
undefined_instruction:
    stmfd sp!, {r0-r3, r12, r14}

    bl disable_interrupt
    bl handle_undefined_instruction
    bl enable_interrupt

    ldmfd sp!, {r0-r3, r12, r15}^

software_interrupt:
    stmfd sp!, {r0-r3, r12, r14}

    bl disable_interrupt
    bl handle_software_interrupt
    bl enable_interrupt

    ldmfd sp!, {r0-r3, r12, r15}^

data_abort:
    stmfd sp!, {r0-r3, r12, r14}

    bl disable_interrupt
    bl handle_data_abort
    bl enable_interrupt

    ldmfd sp!, {r0-r3, r12, r15}^

prefetched_abort:
    stmfd sp!, {r0-r3, r12, r14}

    bl disable_interrupt
    bl handle_prefetched_abort
    bl enable_interrupt

    ldmfd sp!, {r0-r3, r12, r15}^

irq:
    stmfd sp!, {r0-r3, r12, r14}

    bl disable_interrupt
    bl handle_irq
    bl enable_interrupt

    ldmfd sp!, {r0-r3, r12, r15}^

fiq:
    stmfd sp!, {r0-r3, r12, r14}

    bl disable_interrupt
    bl handle_fiq
    bl enable_interrupt

    ldmfd sp!, {r0-r3, r12, r15}^





