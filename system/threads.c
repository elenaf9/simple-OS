#include "system_timer.h"
#include <mem.h>
#include <print.h>
#include <processor.h>
#include <threads.h>

#define NUM_THREADS 16
#define STACK_SIZE 0x1000 // 4kB
#define THREADS_STACK_BOTTOM 0x23000000 

// Since the stack grows towards 0 we can place the TCBS at the same address,
// i.e. behind the bottom of the first thread's stack
#define TCBS_BASE THREADS_STACK_BOTTOM

static int const smoke_test_parameter = 42;

enum thread_state { READY, RUNNING, INACTIVE};

struct list_elem {
  struct list_elem *prev;
  struct list_elem *next;
  struct tcb *data;
};

struct tcb {
  unsigned int id;
  unsigned int sp;
  enum thread_state state;
  struct list_elem element;
};

static struct tcb *const tcbs = (struct tcb *)TCBS_BASE;
struct list_elem *runqueue;

static void enqueue_idle_thread(void) {
  struct tcb *idle_tcb = &tcbs[0];
  idle_tcb->state = READY;
  idle_tcb->element.next = &idle_tcb->element;
  idle_tcb->element.prev = &idle_tcb->element;
  runqueue = &idle_tcb->element;
}

// Idle thread that runs if no other thread is active.
static void idle_thread(int p) {
  if (p != smoke_test_parameter) {
    // Something went wrong with the parameter.
    printf("PANIC: wrong parameter %x!!!\n", p);
    return;
  }
  while (1) {}
}

// Initiate threading and insert an idle thread that
// runs when no other thread is active.
void init_threading(void) {
  // Init state of empty tcbs
  int i;
  for (i = 0; i < NUM_THREADS + 1; i++) {
    tcbs[i].state = INACTIVE;
    tcbs[i].id = i;
    tcbs[i].element.data = &tcbs[i];
  }

  struct tcb *idle_tcb = &tcbs[0];
  // Initialized a stack for this thread so that the correct context is loaded on 
  // thread switch.
  idle_tcb->sp = _init_idle_thread_stack(THREADS_STACK_BOTTOM, &idle_thread, smoke_test_parameter);
  enqueue_idle_thread();
}

static void copy_thread_stack(struct tcb *parent, struct tcb *child) {
  int child_stack_bottom = THREADS_STACK_BOTTOM - (child->id*STACK_SIZE);
  int parent_stack_bottom = THREADS_STACK_BOTTOM - (parent->id*STACK_SIZE);
  int parent_used_stack_size = parent_stack_bottom - parent->sp;

  // Copy the stack of the parent thread to the child.
  child->sp = child_stack_bottom-parent_used_stack_size;

  unsigned int src_addr = parent_stack_bottom;
  unsigned int dst_addr = child_stack_bottom;
  while (dst_addr > child->sp) {
      mem_write_u32(dst_addr, mem_read_u32(src_addr));
      dst_addr -= 4;
      src_addr -= 4;
  }
}

int spawn_thread(unsigned int parent_sp) {
  // Insert at a free slot in the TCBs array.
  int is_space = 0;
  int i;
  for (i = 1; i < NUM_THREADS + 1; i++) {
    if (tcbs[i].state == INACTIVE) {
      is_space = 1;
      break;
    }
  }
  if (!is_space) {
    return -1;
  }

  struct tcb *child = &tcbs[i];
  child->state = READY;
  struct tcb *parent = runqueue->data;
  parent->sp = parent_sp;
  copy_thread_stack(parent, child);

  if (runqueue->data->id == 0) {
    // Only idle thread was running - replace in runqueue.
    child->element.next = &child->element;
    child->element.prev = &child->element;
    runqueue = &child->element;
  } else {
    // Insert as next thread.
    child->element.prev = runqueue;
    child->element.next = runqueue->next;
    runqueue->next->prev = &child->element;
    runqueue->next = &child->element;
  }

  return i;
}


void exit_thread(void) {
  struct list_elem *self = runqueue;
  self->data->state = INACTIVE;

  // If this was the only thread remaining enqueue idle thread
  if (self->next == self) {
    enqueue_idle_thread();
  } else {
    runqueue = self->next;
    runqueue->prev = self->prev;
    runqueue->prev->next = runqueue;
  }
  switch_thread();
}


// Switch to the next thread in the runqueue if there are more than one threads
// active.
void switch_thread(void) {
  if (runqueue->next == runqueue && runqueue->data->state == RUNNING) {
    // Only single thread active -  no thread switch needed.
    return;
  }

  runqueue = runqueue->next;
  runqueue->prev->data->state = READY;
  runqueue->data->state = RUNNING;

  printf("\n");

  runqueue->prev->data->sp = _switch_usr_stack(runqueue->data->sp);
}
