#include "aic.h"
#include "dbgu.h"
#include "print.h"
#include "system_timer.h"
#include "threads.h"

void handle_irq(void) {
  if (is_st_interrupt()) {
    printf("!");
    count_delay();
    thread_switch();
  } else if (is_dbgu_rx_ready()) {
    char c = get_char();
    if (spawn_thread(periodically_print_char, c) == -1) {
      printf("Spawning new thread failed - max number of threads reached.\n");
      return;
    }
  } else {
    printf("Other Interrupt!\n");
  }
}

void handle_data_abort(void) { printf("Data abort!\n"); }

void handle_prefetched_abort(void) { printf("Prefetched abort!\n"); }

int handle_software_interrupt(int num, int arg1, int arg2, int arg3) {
  int ret = 0;
  switch (num) {
  case 1:
    break;
  case 2:
    write((char)arg1);
    break;
  case 3:
    ret = spawn_thread((thread_fn)arg1, arg2);
    break;
  case 4:
    despawn_thread(arg1);
    break;
  case 5:
    thread_delay(arg1);
    break;
  }

  return ret;
}

void handle_undefined_instruction(int addr) {
  printf("Undefined instruction triggered at %x!\n", addr);
}

void handle_fiq(void) { printf("Fast Interrupt!\n"); }

void handle_reset(void) { printf("Resetting...\n"); }
