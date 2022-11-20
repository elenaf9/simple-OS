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
    stmfd sp!, {r0-r12, r14}

    push {r0-r3, lr}
    bl disable_interrupt
    pop {r0-r3, lr}

    push {r0-r3, lr}
    bl interrupt_handler
    pop {r0-r3, lr}


    push {r0-r3, lr}
    bl enable_interrupt
    pop {r0-r3, lr}

    ldmfd sp!, {r0-r12, r15}^
    
undefined_instruction:

    stmfd sp!, {r0-r12, r14}

    push {r0-r3, lr}
    bl disable_interrupt
    pop {r0-r3, lr}

    push {r0-r3,lr}
    bl interrupt_handler
    pop {r0-r3,lr}

    push {r0-r3, lr}
    bl enable_interrupt
    pop {r0-r3, lr}

    ldmfd sp!, {r0-r12, r15}^
    

software_interrupt:


    stmfd sp!, {r0-r12, r14}

    push {r0-r3, lr}
    bl disable_interrupt
    pop {r0-r3, lr}
    
    push {r0-r3,lr}
    bl interrupt_handler
    pop {r0-r3,lr}

    push {r0-r3, lr}
    bl enable_interrupt
    pop {r0-r3, lr} 

    ldmfd sp!, {r0-r12, r15}^
    


handle_exception:




