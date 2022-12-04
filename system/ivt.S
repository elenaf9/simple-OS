.section .ivt, "ax", %progbits
.global irq
ivt:
    b reset
    b undefined_instruction // 0x2c
    b software_interrupt    // 0x30
    b prefetched_abort      // 0x34
    b data_abort            // 0x38
    nop                     // 0x3c
    b irq                   // 0x40
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
    stmfd sp!, {r0-r3, r12, lr}
    bl disable_interrupt
    bl handle_irq
    bl enable_interrupt
    ldmfd sp!, {r0-r3, r12, pc}^


fiq:
    stmfd sp!, {r0-r3, r12, lr}
    bl disable_interrupt
    bl handle_fiq
    bl enable_interrupt
    ldmfd sp!, {r0-r3, r12, pc}^


    







