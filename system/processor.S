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

    stmfd sp!, {r4-r12}

    mov r4, sp
    mov r5, lr
    mov r6, r1
    mov r7, r2
    mov r8, r3

    mov r1, #0x23000000 
    bl _init_stack
    mov r1, sp

    // Create fresh context in the correct order so that 
    // the thread can run once _switch_thread is called for it.

    // Save pointer to thread function in stack
    mov lr, r6
    stmfd sp!, {lr}

    stmfd sp!, {r1}

    // save input parameters for run_thread function
    stmfd sp!, {r7, r8}

    // mov r2, #0b11111 // initial cpu state: no flags set, processor is in user-mode
    // stmfd sp!, {r2} // save state to stack

    mov r0, sp // return stack pointer in r0

    mov lr, r5
    mov sp, r4

    ldmfd sp!, {r4-r12}

    // return
    bx lr

.global _switch_thread
// @r0 where to store the stack pointer of current thread
// @r1 from where to load the stack pointer of the next thread
_switch_thread:
     
    // Store IRQ stack pointer, switch to thread stack instead
    mov r2, sp
    ldr sp, [r0] // load address stored in r0 into sp

    stmfd sp!, {r14} // link register
    stmfd sp!, {r2} // stack
    
    stmfd sp!, {r2-r3} // dummy register save

    // mrs r2, spsr // saved CPU state into r2
    // stmfd sp!, {r2} // and then to Stack

    // Already load address of next thread's sp
    // This is needed because it is possible that r1 and r0 point to the same
    // address (i.e. after init when running the idle thread the first time)
    ldr r1, [r1] 

    str sp, [r0] // tcb->sp = sp
    
    // switch to other process
    mov sp, r1

    // // restore context
    // ldmfd sp!, {r2} // CPU state to r2
    // msr spsr, r2 // and then into saved state

    ldmfd sp!, {r0, r1} // input parameter if thread runs for the first time
    ldmfd sp!, {r2} // restore stack pointer
    ldmfd sp!, {lr} // restore link register

    // Store thread stack, switch to back to IRQ stack
    str sp, [r1] // tcb->sp = sp
    mov sp, r2

    bx lr
