#include <dbgu.h>
#include <exceptions.h>
#include <ivt.h>
#include <print.h>

// Trigger data abort exception.
void trigger_abort(void) {
  printf("test data_abort\n");
  *(volatile unsigned int *)0x90000000 = 0;
  printf("data_abort test successful\n");
  printf("test undefined instruction\n");
  asm("udf");
  printf("undefined instruction test successful\n");
  printf("test software interrupt\n");
  asm("swi 2");
  printf("software interrupt test successful\n");
}

// Test function that echoes received chars via the debug unit.
int main(void) {
  remap_mc();
  init_dbgu();

  // trigger exception
  trigger_abort();

  int i = 0;

  while (1) {
    char val = read_char();
    printf("[%x] %s: %c, pointer:%p\n", i, "Received", val, &val);
    i++;
  }
  reset();
}