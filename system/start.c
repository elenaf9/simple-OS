#include <aic.h>
#include <dbgu.h>
#include <mem.h>
#include <system_timer.h>
#include <tests.h>

// Test function that echoes received chars via the debug unit.
int main(void) {
  remap_mc();
  init_dbgu();
  init_aic();
  init_st();

  trigger_undefined_instruction();

  for (;;);
  reset();
}
