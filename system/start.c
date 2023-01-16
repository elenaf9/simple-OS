#include <aic.h>
#include <dbgu.h>
#include <mem.h>
#include <print.h>
#include <syscalls.h>
#include <system_timer.h>
#include <tests.h>
#include <threads.h>

// Test function that echoes received chars via the debug unit.
int main(void) {
  remap_mc();
  init_aic();
  init_dbgu();
  init_st();

  init_threading();

  read_test();

  for (;;)
    ;

  reset();
}
