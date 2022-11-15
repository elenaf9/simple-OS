#include "main.h"

__attribute__((naked, section(".init"))) void _start(void) {
  main();
}