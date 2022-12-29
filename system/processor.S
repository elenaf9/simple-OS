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

.global _init_stack
// @r0: Stack index
// @r1: Stack bottom
_init_stack:

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

.global _init_thread
// @r0: thread index
// @r1: pointer to run_thread helper function
// @r2: 1st parameter for run_thread: actual thread function
// @r3: 2nd parameter for run_thread: parameter for actual thread
// returns stack pointer
_init_thread:

    stmfd sp!, {r4-r12, r14}

    mov r4, sp
    mov r6, r1
    mov r7, r2
    mov r8, r3

    mov r1, #0x23000000 
    bl _init_stack

    // Create fresh context in the correct order so that 
    // the thread can run once _switch_thread is called for it.

    mov r0, r7
    mov r1, r8 
    mov lr, r6
    stmfd sp!, {r0-r1, r4-r12, lr}

    mrs r2, spsr
    stmfd sp!, {r2} // save state to stack

    mov r0, sp // return stack pointer in r0

    mov sp, r4

    ldmfd sp!, {r4-r12, r14}

    // return
    bx lr

.global _switch_thread
// @r0 where to store the stack pointer of current thread
// @r1 from where to load the stack pointer of the next thread
//
// Partly based on: Betriebssysteme WS 2022/23, Chapter-3, page 3-76
// but adjusted since we are not in IRQ modus here.
_switch_thread:
    
    stmfd sp!, {r2-r12, r14} // store context

    mrs r2, spsr // saved CPU state into r2
    stmfd sp!, {r2} // and then to Stack

    // Already load address of next thread's sp (&tcb->sp).
    // 
    // This is needed because it is possible that r1 and r0 point to the same
    // address (i.e. after init when running the idle thread the first time).
    // In that case the following instruction would overwrite [r1].
    ldr r1, [r1] 

    str sp, [r0] // Write-back stack pointer into the tcb->sp of the old thread's stack
    
    // switch to other process
    mov sp, r1 // Move stack pointer to new stack

    // restore context
    ldmfd sp!, {r2} // CPU state to r2
    msr spsr, r2 // and then into saved state

    // Move to r2 so its not overwritten in the next instruction
    mov r3, r1

    ldmfd sp!, {r0-r1, r4-r12, r14} // restore context

    bx lr
