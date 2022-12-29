.section .init
.global _start
_start:
    // Switch to each processor mode and set stack pointer
    mov r3, #0x24000000  // Stack Bottom

    // Mode FIQ
    mov r0, #0b10001
    bl  _switch_cpu_mode
    mov r0, #0
    mov r1, r3
    bl _init_stack

    // Mode IRQ
    mov r0, #0b10010
    bl  _switch_cpu_mode
    mov r0, #1
    mov r1, r3
    bl _init_stack

    // Mode Abort
    mov r0, #0b10111
    bl  _switch_cpu_mode
    mov r0, #2
    mov r1, r3
    bl _init_stack

    // Mode Undefined
    mov r0, #0b11011
    bl  _switch_cpu_mode
    mov r0, #3
    mov r1, r3
    bl _init_stack 

    // Mode System (also initializes user stack)
    mov r0, #0b11111
    bl  _switch_cpu_mode
    mov r0, #4
    mov r1, r3
    bl _init_stack

    // Mode Supervisor
    mov r0, #0b10011
    bl  _switch_cpu_mode
    mov r0, #5
    mov r1, r3
    bl _init_stack

    // Jump to C main function
    bl main
