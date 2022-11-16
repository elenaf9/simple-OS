#include <dbgu.h>
#include <print.h>

// Test function that echoes received chars via the debug unit.
int main(void) {
  init_dbgu();

  int i = 0;

  while (1) {
    char val = read_char();
    printf("[%x] %s: %c, pointer:%p\n", i, "Received", val, &val);
    i++;
  }
  reset();
}