#include "print.h"

__attribute__((naked, section(".init"))) void _start(void) {
  init_dbgu();
  int i = 0;

  while (1) {
    char val = read_char();
    printf("[%x] %s: %c %p\n", i, "Received", val, &val);
    i++;
  }
  reset();
}