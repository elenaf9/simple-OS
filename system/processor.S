.section .text

.global _switch_cpu_mode
// @r0: CPU mode
_switch_cpu_mode:
    mov r1, lr

    // Set stack mode
    mrs r2, cpsr
    and r2, #0xFFFFFFE0
    orr r2, r0
    msr cpsr, r2

    bx r1

.global _init_cpu_stack
// @r0: Stack index
_init_cpu_stack:
    // Stack bottom
    mov r1, #0x24000000

    // Stack size: 64kB
    mov r2, #(1024*64)
    mul r0, r2, r0

    // Set stack pointer
    sub r1, r1, r0
    mov sp, r1

    bx lr

.global _cpu_disable_interrupt
_cpu_disable_interrupt:
    mrs r1, cpsr
    mov r0, #0xC0
    orr r1, r0
    msr cpsr, r1

    bx lr

.global _cpu_enable_interrupt
_cpu_enable_interrupt:
    mrs r1, cpsr
    mov r0, #0xFFFFFF3F
    and r1, r0
    msr cpsr, r1

    bx lr

.global _init_thread_stack
// Initializes a stack for a thread so that the correct context is loaded in 
// _switch_thread.
// 
// @r0: stack bottom for this thread
// @r1: pointer to `run_thread` helper function
// @r2: 1st parameter for run_thread: the thread's actual function
// @r3: 2nd parameter for run_thread: parameter for the thread's function
// returns stack pointer
_init_thread_stack:

    stmfd sp!, {r4, lr}
    mov r4, sp

    // Init stack for thread at the given bottom
    mov sp, r0 

    // == State that will be popped in _irq.

    // Pointer to `run_thread`.
    // Will be loaded into program counter.
    mov r14, r1
    stmfd sp!, {r14} 
    
    // Parameters for `run_thread`. 
    mov r0, r2
    mov r1, r3

    stmfd sp!, {r1-r12, r14} 
    stmfd sp!, {r0} 
    // Initial processor status register
    mov r1, #0b10000
    stmfd sp!, {r1}

    // Return  stack pointer in r0
    mov r0, sp

    mov sp, r4
    ldmfd sp!, {r4, pc}


.global _switch_thread
// Switch context for threads.
//
// @r0 the usr_sp of the next thread
//
// returns the usr_sp of the previous thread
_switch_thread:

    stmfd sp!, {r13}^
    ldmfd sp!, {r1}

    stmfd sp!, {r0}
    ldmfd sp!, {r13}^

    mov r0, r1
    
    bx lr
