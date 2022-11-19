.section .ivt, "ax", %progbits
ivt:
    nop
    b undefined_instruction
    b handle_exception
    b handle_exception
    b data_abort
    nop
    b handle_exception
    b handle_exception

data_abort:
    stmfd sp!, {r0-r12, r14}
    bl interrupt_handler
    ldmfd sp!, {r0-r12, r15}^

undefined_instruction:
    stmfd sp!, {r0-r12, r14}
    bl interrupt_handler
    ldmfd sp!, {r0-r12, r15}^

handle_exception:




