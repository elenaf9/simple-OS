#include <stddef.h>
#include <stdint.h>
#include <threads.h>

#define NUM_THREADS 16
#define THREAD_STACK_SIZE 4096

uint8_t current_thread = 0;
uint8_t thread_idx = 0;
uint8_t thread_stacks[NUM_THREADS][THREAD_STACK_SIZE];

struct pcb pcb_array[NUM_THREADS];
unsigned int pcb_idx = 0;

void create_pcb(size_t id, size_t pc, size_t sp) {
  struct pcb new_pcb = {id, pc, sp};
  pcb_array[pcb_idx] = new_pcb;
}

void idle_thread(void) {
  while (1) {
    printf("idle\n");
  }
}

void init_idle_thread(void) {
  create_pcb(thread_idx, (size_t)&idle_thread,
             (size_t)(thread_stacks[thread_idx] + THREAD_STACK_SIZE - 1));
  thread_idx += 1;
}

void thread_switch(void) {
  if (thread_idx - 1 == current_thread) {
    current_thread = 0;
  } else {
    current_thread += 1;
  }
}

void create_thread(size_t thread_pc) {
  create_pcb(thread_idx, thread_pc,
             (size_t)(thread_stacks[thread_idx] + THREAD_STACK_SIZE - 1));

  thread_idx += 1;
}
