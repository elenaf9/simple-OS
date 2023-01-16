.section .init
.global _start
_start:
    // Switch to each processor mode and set stack pointer

    // Mode System (also initializes user stack).
    // The system mode stack is initialized at index 0 of all processor
    // mode stacks. It is important that this does not change! We use the
    // hardcoded value at some other places in the code. 
    mov r0, #0b11111
    bl  _switch_cpu_mode
    mov r0, #0
    bl _init_cpu_stack

    // Mode FIQ
    mov r0, #0b10001
    bl  _switch_cpu_mode
    mov r0, #1
    bl _init_cpu_stack

    // Mode IRQ
    mov r0, #0b10010
    bl  _switch_cpu_mode
    mov r0, #2
    bl _init_cpu_stack

    // Mode Abort
    mov r0, #0b10111
    bl  _switch_cpu_mode
    mov r0, #3
    bl _init_cpu_stack

    // Mode Undefined
    mov r0, #0b11011
    bl  _switch_cpu_mode
    mov r0, #4
    bl _init_cpu_stack 

    // Mode Supervisor
    mov r0, #0b10011
    bl  _switch_cpu_mode
    mov r0, #5
    bl _init_cpu_stack

    // Jump to C main function
    bl main
