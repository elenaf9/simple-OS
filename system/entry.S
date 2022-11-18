.section .init
.global _start
_start:
    // Switch to each processor mode and set stack pointer
    mrs r0, cpsr
    and r0, #0xFFFFFFE0
    orr r0, #0b10001
    msr cpsr, r0
    ldr sp, =0x00200200

    mrs r0, cpsr
    and r0, #0xFFFFFFE0
    orr r0, #0b10010
    msr cpsr, r0
    ldr sp, =0x00200400
    
    mrs r0, cpsr
    and r0, #0xFFFFFFE0
    orr r0, #0b10111
    msr cpsr, r0
    ldr sp, =0x00200600
    
    mrs r0, cpsr
    and r0, #0xFFFFFFE0
    orr r0, #0b11011
    msr cpsr, r0
    ldr sp, =0x00200800
    
    mrs r0, cpsr
    and r0, #0xFFFFFFE0
    orr r0, #0b11111
    msr cpsr, r0
    ldr sp, =0x00200100

    mrs r0, cpsr
    and r0, #0xFFFFFFE0
    orr r0, #0b10011
    msr cpsr, r0
    ldr sp, =0x00200c00
    bl main

.Lend:
b .Lend


