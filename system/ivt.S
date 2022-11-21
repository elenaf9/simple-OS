.section .ivt, "ax", %progbits
ivt:
    nop
    b undefined_instruction
    b software_interrupt
    b handle_exception
    b data_abort
    nop
    b handle_exception
    b handle_exception

data_abort:
    stmfd sp!, {r0-r3, r12, r14}

    bl disable_interrupt
    bl interrupt_handler
    bl enable_interrupt

    ldmfd sp!, {r0-r3, r12,  r15}^
    
undefined_instruction:
    stmfd sp!, {r0-r3, r12, r14}
    
    bl disable_interrupt
    bl interrupt_handler
    bl enable_interrupt

    ldmfd sp!, {r0-r3, r12,  r15}^
    

software_interrupt:
    stmfd sp!, {r0-r3, r12, r14}
    
    bl disable_interrupt
    bl interrupt_handler
    bl enable_interrupt

    ldmfd sp!, {r0-r3, r12,  r15}^

handle_exception:




