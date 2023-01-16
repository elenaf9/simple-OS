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

.global _init_idle_thread_stack
// Initializes a stack for a thread so that the correct context is loaded in 
// _switch_usr_stack.
// 
// @r0: stack bottom for this thread
// @r1: pointer to the idle thread's function
// @r2: sanity check parameter for idle thread function.
// returns stack pointer
_init_idle_thread_stack:

    stmfd sp!, {r4, lr}
    mov r4, sp

    // Init stack for thread at the given bottom
    mov sp, r0 

    // == State that will be popped in _irq.

    // Pointer to `run_thread`.
    // Will be loaded into program counter.
    mov r14, r1
    stmfd sp!, {r14} 
    
    // Parameter for idle thread. 
    mov r0, r2

    stmfd sp!, {r1-r12, r14} 
    // Initial processor status register
    mov r1, #0b10000
    stmfd sp!, {r1}

    stmfd sp!, {r0} 

    // Return  stack pointer in r0
    mov r0, sp

    mov sp, r4
    ldmfd sp!, {r4, pc}


.global _switch_usr_stack
// Switch context for threads.
//
// @r0 the usr_sp of the next thread
//
// returns the usr_sp of the previous thread
_switch_usr_stack:

    stmfd sp!, {r13}^
    ldmfd sp!, {r1}

    stmfd sp!, {r0}
    ldmfd sp!, {r13}^

    mov r0, r1
    
    bx lr


// Store all registers except r0, and the spsr on the user stack.
// r0 can not be stored since it needs to carry the LR value.
// It can be stored in a follow-up call to `_push_r0_usr`.
// 
// @r0: The LR register that should be stored
//
.global _push_usr_ctx
_push_usr_ctx:
    stmfd sp!, {lr}
    mov lr, r0

    // Load usr_sp into r0 and save context on it.
    stmfd sp!, {r13}^
    ldmfd sp!, {r0}

    // Store context
    stmfd r0!, {r14}^
    stmfd r0!, {r1-r12, r14}

    mrs r1, spsr
    stmfd r0!, {r1}

    stmfd sp!, {r0}
    ldmfd sp!, {r13}^

    ldmfd sp!, {pc}

.global _pop_usr_ctx
_pop_usr_ctx:
    stmfd sp!, {lr}

    // Load usr_sp into r0 and restore context from it.
    stmfd sp!, {r13}^
    ldmfd sp!, {r0}

    // Restore context

    // Temporary save r1, which will be later loaded into r0.
    ldmfd r0!, {r1}
    stmfd sp!, {r1}

    // Restore spsr
    ldmfd r0!, {r1}
    msr spsr, r1

    ldmfd r0!, {r1-r12, r14}
    ldmfd r0!, {r14}^

    stmfd sp!, {r0}
    ldmfd sp!, {r13}^

    ldmfd sp!, {r0}

    ldmfd sp!, {pc}


// @r0: The r0 register that should be stored
.global _push_r0_usr
_push_r0_usr:
    mov r1, r0
    stmfd sp!, {r13}^
    ldmfd sp!, {r0}

    stmfd r0!, {r1}

    stmfd sp!, {r0}
    ldmfd sp!, {r13}^

    bx lr

