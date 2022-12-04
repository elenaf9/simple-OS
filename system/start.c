#include <aic.h>
#include <dbgu.h>
#include <mem.h>
#include <system_timer.h>
#include <tests.h>
#include <print.h>

// Test function that echoes received chars via the debug unit.
int main(void) {
  remap_mc();
  init_aic();
  init_dbgu();
  init_st();

  for (;;);

  reset();
}
