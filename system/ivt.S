.section .text
ivt:
    bl handle_rst
    bl handle_und
    bl handle_swi
    bl handle_prefetch_abort
    bl handle_data_abort
    bl handle_data_abort // dummy instruction to keep offset
    bl handle_irq
    bl handle_fiq

.global test_asm
test_asm:
    stmfd sp!, {r1, lr}
    bl interrupt_handler
    ldmfd sp!, {r1, lr}
    bx lr

handle_exception:
    stmfd sp!, {r4-r11, r15}
    bl interrupt_handler
    ldmfd sp!, {r4-r11, r15}^

handle_rst:
    bl interrupt_handler;

handle_und:
    bl handle_exception

handle_swi:
    bl handle_exception

handle_prefetch_abort:
    bl handle_exception

handle_data_abort:
    bl handle_exception

handle_irq:
    bl handle_exception

handle_fiq:
    stmfd sp!, {r4-r7, r15}
    bl interrupt_handler
    ldmfd sp!, {r4-r7, r15}^