#include <dbgu.h>
#include <exceptions.h>
#include <ivt.h>
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

// Test function that echoes received chars via the debug unit.
int main(void) {
  remap_mc();
  init_dbgu();

  printf("## Test exception handling ##\nThe following chars are accepted for triggering different exceptions:\n");
  printf("- 'a': Trigger data abort exception.\n");
  printf("- 'u': Trigger undefined-instruction exception.\n");
  printf("- 's': Trigger software interrupt.\n\n");

  while (1) {
    char val = read_char();
    switch (val) {
      case 'a':
        trigger_abort();
        break;
      case 'u':
        trigger_undefined_instruction();
        break;
      case 's':
        trigger_software_interrupt();
        break;
      default:
        printf("Invalid input %c.\nOnly 'a', 'u' and 's' are supported.\n\n", val);
        continue;
    }
    printf("...continuing program.\n\n");
  }
  reset();
}