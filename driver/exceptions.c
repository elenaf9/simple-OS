#include <print.h>
#define USER_I 0xFFFFFF00 // User Interface

#define MC_RCR (1 << 0) // Memory Control Remap Control Register -  Write-only

// Remap memory controller.
// Causes the Internal RAM to be accessed through the Internal Memory Area 0.
void remap_mc(void) { *(volatile unsigned int *)USER_I = MC_RCR; }

void disable_interrupt(void) {
  asm volatile("mrs r1, cpsr;"
               "mov r0, #0xC0;"
               "orr r1, r0;"
               "msr cpsr, r1;");
}

void enable_interrupt(void) {
  asm volatile("mrs r1, cpsr;"
               "mov r0, #0xFFFFFF3F;"
               "and r1, r0;"
               "msr cpsr, r1;");
}

void handle_data_abort(void) {
  printf("Data abort!\n");
}

void handle_prefetched_abort(void) {
  printf("Prefetched abort!\n");

}
void handle_software_interrupt(void) {
  printf("Software interrupt!\n");

}

void handle_undefined_instruction(void) {
  printf("Undefined instruction!\n");
}

void handle_irq(void)  {
  printf("Interrupt!\n");
}

void handle_fiq(void) {
  printf("Fast Interrupt!\n");
}

void handle_reset(void) {
  printf("Resetting...\n");
}
