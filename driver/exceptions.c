#include "aic.h"
#include "dbgu.h"
#include "print.h"
#include "system_timer.h"
#include "threads.h"
#include "syscalls.h"

void handle_irq(void) {
  switch (is_st_interrupt()) {
    case PERIOD_TIMER:
      switch_thread();
      break;
    case ALARM:
      on_st_alarm();
      break;
    default:
      break;
  }

  if (is_dbgu_rx_ready()) {
    char c = get_char();
    on_dbgu_rx(c);
  }
}

void handle_data_abort(int addr) {
  printf("Data abort triggered at %x!\n", addr);
}

void handle_prefetched_abort(int addr)  {
  printf("Prefetched abort triggered at %x!\n", addr);
}



void handle_software_interrupt(void) { printf("Software interrupt!\n"); }

void handle_undefined_instruction(int addr) {
  printf("Undefined instruction triggered at %x!\n", addr);
}

void handle_fiq(void) { printf("Fast Interrupt!\n"); }

void handle_reset(void) { printf("Resetting...\n"); }
