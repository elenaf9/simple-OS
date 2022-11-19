#include <print.h>
#define USER_I 0xFFFFFF00 // User Interface

#define MC_RCR (1<<0) // Memory Control Remap Control Register -  Write-only

// Remap memory controller.
// Causes the Internal RAM to be accessed through the Internal Memory Area 0.
void remap_mc(void) {
    *(volatile unsigned int *)USER_I = MC_RCR;
}

void interrupt_handler(void) {
    printf("Interrupt!\n");
}
