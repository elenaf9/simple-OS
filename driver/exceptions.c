#include <print.h>
#define USER_I 0xFFFFFF00 // User Interface

#define MC_RCR (1 << 0) // Memory Control Remap Control Register -  Write-only

// Remap memory controller.
// Causes the Internal RAM to be accessed through the Internal Memory Area 0.
void remap_mc(void) { *(volatile unsigned int *)USER_I = MC_RCR; }

void disable_interrupt(void) {
  asm volatile("push {r0,r1};"
               "mrs r1, cpsr;"
               "mov r0, #0xC0;"
               "orr r1, r0;"
               "msr cpsr, r1;"
               "pop {r0, r1};");
}

void enable_interrupt(void) {
  asm volatile("push {r0, r1};"
               "mrs r1, cpsr;"
               "mov r0, #0xFFFFFF3F;"
               "and r1, r0;"
               "msr cpsr, r1;"
               "pop {r0, r1};");
}

void interrupt_handler(void) { printf("Interrupt!\n"); }
