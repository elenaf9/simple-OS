.section .ivt, "ax", %progbits
ivt:
    nop
    b handle_exception
    b handle_exception
    b handle_exception
    b handle_exception
    nop
    b handle_exception
    b handle_exception

handle_exception:
    stmfd sp!, {r0-r12, r14}
    bl interrupt_handler
    ldmfd sp!, {r0-r12, r15}^



