#include "print.h"
#include "dbgu.h"
#include "system_timer.h"

void handle_data_abort(void) { 
  printf("Data abort!\n");
}

void handle_prefetched_abort(void) {
  printf("Prefetched abort!\n");
}

void handle_software_interrupt(void) {
  printf("Software interrupt!\n");
}

void handle_undefined_instruction(int addr) {
  printf("Undefined instruction triggered at %x!\n", addr);
}

void handle_irq(void) {
  if (is_st_interrupt()) {
    printf("!\n");
  } else if (is_dbgu_rx_ready()) {
    char c = get_char();
    int asci_code = c;
    int i;
    for (i = 0; i < asci_code; i++) {
      printf("%c", c);
      busy_wait(200); // wait 0.2s
    }
  } else {
    printf("Interrupt!\n");
  }
}

void handle_fiq(void) {
  printf("Fast Interrupt!\n");
}

void handle_reset(void) {
  printf("Resetting...\n");
}
