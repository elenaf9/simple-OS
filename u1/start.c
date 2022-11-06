#include "print.h"

// Test function that echoes received chars via the debug unit.
__attribute__((naked, section(".init"))) void _start(void) {
  init_dbgu();
  int i = 0;

  while (1) {
    char val = read_char();
    printf("[%x] %s: %c\n", i, "Received", val);
    i++;
  }
  reset();
}