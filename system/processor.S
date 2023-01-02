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

    stmfd sp!, {r4-r8, lr}
    mov r4, sp
    mov r6, r1
    mov r7, r2
    mov r8, r3

    // Init stack for thread at the given bottom
    mov sp, r0 
    
    // Bottom of this thread's user mode stack.
    // Offset 512B so that the first 512B can be used
    // as stack in the IRQ mode
    sub r2, sp, #0x200


    // == State that will be popped in _irq.

    // Pointer to `run_thread`.
    // Will be loaded into program counter.
    stmfd sp!, {r6} 

    // Will be loaded into {r0-r3, r12}.
    // r0 and r1 are parameters for the thread function, 
    // rest is dummy.
    stmfd sp!, {r7-r10, r12} 


    // == State that will be popped in _switch_thread.

    // Set link register to directly jump to _end_irq.
    // Will be popped into pc at end of _switch_thread.
    ldr lr, _jt_end_irq
    stmfd sp!, {lr}


    // Use newly initiated stack, which will be loaded into SP_usr
    stmfd sp!, {r2}
    // Dummy user register (won't be read since thread runs for the first time)
    stmfd sp!, {r0-r12, r14}


    // Initialized system processor mode for thread.
    mov r3, #0b10000
    stmfd sp!, {r3}

    // Return (IRQ-mode) stack pointer in r0
    mov r0, sp


    mov sp, r4
    ldmfd sp!, {r4-r8, pc}


_jt_end_irq: .word _end_irq

.global _switch_thread
// Switch context for threads.
//
// @r0 where to store the stack pointer of current thread.
// @r1 from where to load the stack pointer of the next thread.
//
// Source: partly based on Betriebssysteme WS 2022/23, Chapter-3, page 3-76.
_switch_thread:

    stmfd sp!, {r14}

    // User registers.
    stmfd sp!, {r13}^
    stmfd sp!, {r0-r12, r14}^

    // Saved CPU state.
    mrs r2, spsr
    stmfd sp!, {r2}

    // Already load address of next thread's sp (&tcb->sp).
    // 
    // This is needed because it is possible that r1 and r0 point to the same
    // address (i.e. after init when running the idle thread the first time).
    // In that case the subsequent instruction would overwrite [r1].
    ldr r2, [r1] 
    
    // Write-back stack pointer into the tcb->sp of the old thread's stack
    str sp, [r0]
    

    // == Switch to other process and restore the context.
    //
    // If a thread runs for the first time, this is the context that was prepared in
    // `_init_thread_stack`. 
    // If the thread has previously run this is the context that was saved the last time 
    // this thread was switched out for another thread (with the code above).

    // Move stack pointer to new stack
    mov sp, r2 

    // Saved CPU state
    ldmfd sp!, {r2}
    msr spsr, r2


    // User registers
    ldmfd sp!, {r0-r12, r14}^
    ldmfd sp!, {r13}^

    // Load stored link-register into pc.
    ldmfd sp!, {r15}
