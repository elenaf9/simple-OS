#include <aic.h>
#include <dbgu.h>
#include <mem.h>
#include <print.h>
#include <syscalls.h>
#include <system_timer.h>
#include <tests.h>
#include <threads.h>
#include <processor.h>

// Test function that echoes received chars via the debug unit.
int main(void) {
  remap_mc();
  init_aic();
  init_dbgu();
  init_st();

  init_threading();

  _switch_cpu_mode(0b10000);


  if (fork() == 0)  {
    while (1) {
      char c = read();
      if (fork() == 0) {
        periodically_print_char(c);
        exit();
      }
    }
  }
  for (;;);

  return 0;
}
