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
    stmfd sp!, {r0-r4, r12, lr}
    mov r4, lr
    sub r4, #4 // Instruction that triggered the exception.
    bl disable_interrupt
    mov r0, r4
    bl handle_undefined_instruction
    bl enable_interrupt
    ldmfd sp!, {r0-r4, r12, pc}^

software_interrupt:
    stmfd sp!, {lr}
    bl handle_software_interrupt
    ldmfd sp!, {pc}^

data_abort:
    stmfd sp!, {r0-r3, r12, lr}
    bl disable_interrupt
    bl handle_data_abort
    bl enable_interrupt
    ldmfd sp!, {r0-r3, r12, pc}^

prefetched_abort:
    stmfd sp!, {r0-r3, r12, lr}
    bl disable_interrupt
    bl handle_data_abort
    bl enable_interrupt
    ldmfd sp!, {r0-r3, r12, pc}^

irq:
    //stmfd sp!, {r0-r3, r12, lr}
    //bl disable_interrupt
    //bl handle_irq
    //bl enable_interrupt
    //ldmfd sp!, {r0-r3, r12, pc}^
    ldr pc,[pc,#-0xF20]

    sub r14, r14, #4
    stmfd sp!, {r14}

    mrs r14,spsr
    stmfd sp!, {r0,r14}

    mrs r14, cpsr
    bic r14, r14, #0x80 //set the I bit
    orr r14, r14, #0x1F //switch to system mode
    msr cpsr, r14

    stmfd sp!, {r1-r3, r12, r14}

    bl enable_interrupt

    ldmfd sp!, {r1-r3, r12, r14}

    mrs r0,cpsr
    bic r0, r0, #0x1F
    orr r0, r0, #0x80
    msr cpsr, r0

    ldr r0, = 0xFFFFF000
    str r0, [r0, #0x0130]

    ldmfd sp!, {r0,r14}
    msr spsr, r14

    ldmfd sp!, {pc}^


fiq:
    stmfd sp!, {r0-r3, r12, lr}
    bl disable_interrupt
    bl handle_fiq
    bl enable_interrupt
    ldmfd sp!, {r0-r3, r12, pc}^


    







