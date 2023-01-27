#include <print.h>

// Trigger data abort exception.
void trigger_abort(void) {
  printf("Triggering data abort...\n");
  *(volatile unsigned int *)0x90000000 = 0;
}

// Trigger undefined instruction.
void trigger_undefined_instruction(void) {
  printf("Triggering undefined instruction...\n");
  asm("udf");
}

// Trigger software interrupt.
void trigger_software_interrupt(void) {
  printf("Triggering software interrupt...\n");
  asm("swi 2");
}
