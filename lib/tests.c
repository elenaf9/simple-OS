#include <print.h>
#include <syscalls.h>

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

void read_test() {
  while (1) {
    char c = 'd'; // read();

    printf("%c\n", c);
    delay_thread(32000);
  }
}
