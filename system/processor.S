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

    stmfd sp!, {r4-r8, lr}

    mov r4, sp
    mov r6, r1
    mov r7, r2
    mov r8, r3

    // Init stack for thread
    mov r1, #0x23000000 
    bl _init_stack  
    
    // Bottom of this thread's user mode stack
    // Offset 64B so that the first 64B can be used
    // as stack in the IRQ mode
    sub r2, sp, #0x100

    // == State that will be popped in _irq.

    // Pointer to run_thread.
    // Will be loaded into program counter
    stmfd sp!, {r6} 

    // Will be loaded into {r0-r3, r12}
    // r0 and r1 are parameters for the thread function, 
    // rest is dummy.
    stmfd sp!, {r7-r10, r12} 


    // == State that will be popped int _switch_thread
    //
    // Create fresh context in the correct order so that 
    // the thread can run once _switch_thread is called for it.

    // Will be popped into pc at end of _switch_thread.
    ldr lr, _jt_end_irq
    stmfd sp!, {lr}


    // Use newly initiated stack, which will be popped into SP_usr
    stmfd sp!, {r2}
    // dummy user register (won't be read since thread runs for the first time)
    stmfd sp!, {r0-r12, r14}

    mov r3, #0b10000 // Run thread in user mode
    stmfd sp!, {r3} // save state to stack

    mov r0, sp // return (IRQ-mode) stack pointer in r0

    mov sp, r4

    ldmfd sp!, {r4-r8, pc}


_jt_end_irq: .word _end_irq

.global _switch_thread
// @r0 where to store the stack pointer of current thread
// @r1 from where to load the stack pointer of the next thread
//
// Partly based on: Betriebssysteme WS 2022/23, Chapter-3, page 3-76
// but adjusted since we are not in IRQ modus here.
_switch_thread:

    stmfd sp!, {r14}

    // user registers
    stmfd sp!, {r13}^
    stmfd sp!, {r0-r12, r14}^

    // saved CPU state into R2
    mrs r2, spsr
    stmfd sp!, {r2}

    // Already load address of next thread's sp (&tcb->sp).
    // 
    // This is needed because it is possible that r1 and r0 point to the same
    // address (i.e. after init when running the idle thread the first time).
    // In that case the following instruction would overwrite [r1].
    ldr r2, [r1] 
    
    // Write-back stack pointer into the tcb->sp of the old thread's stack
    str sp, [r0]
    

    // == Switch to other process

    mov sp, r2 // Move stack pointer to new stack

    // CPU state
    ldmfd sp!, {r2}
    msr spsr, r2


    // user registers
    ldmfd sp!, {r0-r12, r14}^
    ldmfd sp!, {r13}^

    ldmfd sp!, {r15}
