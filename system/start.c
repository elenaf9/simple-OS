#include <dbgu.h>
#include <print.h>
#include <exceptions.h>
#include <ivt.h>


// Trigger data abort exception.
void trigger_abort(void) {
    printf("before\n");
  *(volatile unsigned int *)0x90000000 = 0;
    printf("after\n");
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