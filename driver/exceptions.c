#include "aic.h"
#include "dbgu.h"
#include "print.h"
#include "system_timer.h"
#include "threads.h"
#include "sys_calls.h"

void handle_irq(void) {
  if (is_st_interrupt()) {
    printf("!");
    thread_switch();
  } else if (is_dbgu_rx_ready()) {
    char c = get_char();
    int pid = fork();
    if (pid == 0) {
      periodically_print_char(c);
      for (;;);
    } else if (pid < 0) {
      printf("Spawning new thread failed - max number of threads reached.\n");
      return;
    }
  } else {
    printf("Other Interrupt!\n");
  }
}

void handle_data_abort(void) { printf("Data abort!\n"); }

void handle_prefetched_abort(void) { printf("Prefetched abort!\n"); }

void handle_software_interrupt(void) { printf("Software interrupt!\n"); }

void handle_undefined_instruction(int addr) {
  printf("Undefined instruction triggered at %x!\n", addr);
}

void handle_fiq(void) { printf("Fast Interrupt!\n"); }

void handle_reset(void) { printf("Resetting...\n"); }
