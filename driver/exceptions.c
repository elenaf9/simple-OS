#include "aic.h"
#include "dbgu.h"
#include "print.h"
#include "system_timer.h"
#include "threads.h"

void handle_data_abort(void) { printf("Data abort!\n"); }

void handle_prefetched_abort(void) { printf("Prefetched abort!\n"); }

void handle_software_interrupt(void) { printf("Software interrupt!\n"); }

void handle_undefined_instruction(int addr) {
  printf("Undefined instruction triggered at %x!\n", addr);
}

void print_char(int c) {
  int asci_code = c;
  int i;
  for (i = 0; i < asci_code; i++) {
    printf("%c", (char)c);
    busy_wait(100); // wait 0.5s
  }
}

void handle_irq(void) {
  int is_st = is_st_interrupt();
  int is_dbgu_rx = is_dbgu_rx_ready();
  char c;
  if (is_dbgu_rx) {
    c = get_char();
  }

  end_of_interrupt();

  if (is_st) {
    printf("!\n");
    thread_switch();
  }
  if (is_dbgu_rx) {
    if (spawn_thread(print_char, (int)c)) {
      printf("Spawning thread failed!!");
      return;
    }
  }
  if (!is_st && !is_dbgu_rx) {
    printf("Other Interrupt!\n");
  }
}

void handle_fiq(void) { printf("Fast Interrupt!\n"); }

void handle_reset(void) { printf("Resetting...\n"); }
