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

enum thread_state { CREATED, ACTIVE, FINISHED, FREE, WAITING, SLEEPING };

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
  unsigned int delay;
};

static struct tcb *const tcbs = (struct tcb *)TCBS_BASE;

struct list_elem *runqueue;

static int const smoke_test_parameter = 42;

// Type of the functions that are executed in a thread.
typedef void (*thread_fn)(int);

static void run_thread(thread_fn thread_function, int thread_parameter) {

  runqueue->data->state = ACTIVE;

  // Run thread.
  thread_function(thread_parameter);

  runqueue->data->state = FINISHED;

  // If this was the only thread remaining enqueue idle thread
  if (runqueue->next == runqueue) {
    tcbs[0].element.prev = runqueue;
    tcbs[0].element.next = runqueue;
    runqueue->next = &tcbs[0].element;
    runqueue->prev = &tcbs[0].element;
  }

  for (;;)
    ;
}

// Initialize the TCB at `index` in the TCB list so it runs the
// `thread_function` with the `thread_parameter` once the thread is next in the
// runqueue.
//
// Note: tcb.list_elem.{prev,next} are not set and need to be configured
// depending on where the thread is inserted in the runqueue.
static void init_tcb(unsigned int index, thread_fn thread_function,
                     int thread_parameter) {

  struct tcb *new_tcb = &tcbs[index];
  int stack_bottom = (THREADS_STACK_BOTTOM - index * STACK_SIZE);
  // Initialized a stack for this thread so that the correct context is loaded
  // on thread switch.
  new_tcb->sp = _init_thread_stack(stack_bottom, &run_thread, thread_function,
                                   thread_parameter);
  new_tcb->id = index;
  new_tcb->state = CREATED;
  new_tcb->element.data = new_tcb;

  return;
}

// Idle thread that runs if no other thread is active.
//
// Started with a dummy parameter to smoke-test that spawning
// threads with a parameter properly works.
static void idle_thread(int p) {
  if (p != smoke_test_parameter) {
    // Something went wrong with the parameter.
    printf("PANIC: wrong parameter %x!!!\n", p);
    return;
  }
  while (1) {
  }
}

// Initiate threading and insert an idle thread that
// runs when no other thread is active.
void init_threading(void) {

  // Init idle thread
  init_tcb(0, &idle_thread, smoke_test_parameter);

  struct tcb *new_tcb = &tcbs[0];
  new_tcb->element.next = &new_tcb->element;
  new_tcb->element.prev = &new_tcb->element;
  new_tcb->state = CREATED;
  runqueue = &new_tcb->element;

  // Init state of empty tcbs
  int i;
  for (i = 1; i < NUM_THREADS + 1; i++) {
    tcbs[i].state = FREE;
    tcbs[i].id = i;
  }
}

// Spawn a new thread that executes the `thread_function` with the
// `thread_parameter` as function parameter.
int spawn_thread(unsigned int parent_sp) {

  // Insert at a free slot in the TCBs array.
  int is_space = 0;
  int i;
  for (i = 1; i < NUM_THREADS + 1; i++) {
    if (tcbs[i].state == FREE) {
      is_space = 1;
      break;
    }
  }
  if (!is_space) {
    return -1;
  }
  int child_index = i;

  struct tcb *new_tcb = &tcbs[child_index];

  new_tcb->id = child_index;
  new_tcb->state = CREATED;
  new_tcb->element.data = new_tcb;

  int parent_index = runqueue->data->id;
  int parent_stack_bottom = THREADS_STACK_BOTTOM - (parent_index*STACK_SIZE);
  int parent_used_stack_size = parent_stack_bottom - parent_sp;
  int child_stack_bottom = THREADS_STACK_BOTTOM - (child_index*STACK_SIZE);
  
  // printf("child: %x\n", child_index);
  // printf("sp: %x\n", parent_sp);

  // printf("parent STACK_BOTTOM: %x, sp:%x stack_size: %x\n", parent_stack_bottom, parent_sp, parent_used_stack_size);
  // printf("child: %x\n", child_index);
  // printf("sp: %x\n", parent_sp);

  // Copy the stack of the parent thread to the child.
  new_tcb->sp = child_stack_bottom-parent_used_stack_size;

  unsigned int src_addr = parent_stack_bottom;
  unsigned int dst_addr = child_stack_bottom;
  while (dst_addr > new_tcb->sp) {
      mem_write_u32(dst_addr, mem_read_u32(src_addr));
      dst_addr -= 4;
      src_addr -= 4;
  }

  // Insert as next thread except for threads that themselves
  // did not run yet.
  struct list_elem *insert_after = runqueue;
  while (insert_after->next != runqueue) {
    if (insert_after->data->state != CREATED) {
      // Position to be inserted.
      break;
    }
    insert_after = insert_after->next;
  }
  new_tcb->element.prev = insert_after;
  new_tcb->element.next = insert_after->next;
  new_tcb->element.prev->next = &new_tcb->element;
  new_tcb->element.next->prev = &new_tcb->element;

  return child_index;
}

// Switch to the next thread in the runqueue if there are more than one threads
// active.
void thread_switch(void) {

  if (runqueue->next == runqueue && runqueue->data->state == ACTIVE) {
    // Only idle thread active -  no thread switch needed.
    return;
  }

  unsigned int *prev_sp = &runqueue->data->sp;
  runqueue = runqueue->next;
  printf("\n");

  // If current thread is Finished
  if (runqueue->data->state == FINISHED) {
    runqueue = runqueue->next;
  }

  // If previous thread finished mark as free.
  if (runqueue->prev->data->state == FINISHED) {
    runqueue->prev->data->state = FREE;
  }

  // If prev thread was idle thread or is free (=finished), remove from queue.
  if (runqueue->prev->data->id == 0 || runqueue->prev->data->state == FREE) {
    runqueue->prev = runqueue->prev->prev;
    runqueue->prev->next = runqueue;
  }

  *prev_sp = _switch_thread(runqueue->data->sp);
}

void change_thread_wait() {
  runqueue->data->state = WAITING;

  thread_switch();
}

void despawn_thread(int thread_id) {
  struct list_elem *tmp;
  tmp = runqueue;

  while (tmp->data->id != thread_id) {
    tmp = tmp->next;
  }

  tmp->data->state = FINISHED;
}

void thread_delay(unsigned int timer) {
  runqueue->data->state = SLEEPING;
  unsigned int delay = timer / 4000;
  runqueue->data->delay = delay;
}

void count_delay() {
  struct list_elem *tmp;
  tmp = runqueue->next;

  while (tmp != runqueue) {
    if (tmp->data->state == SLEEPING) {
      tmp->data->delay -= 1;
    }
    tmp = tmp->next;
  }
}

void thread_wait() { runqueue->data->state = WAITING; }

void check_wait() {
  struct list_elem *tmp;
  tmp = runqueue->next;

  while (tmp != runqueue) {
    if (tmp->data->state == WAITING) {
      tmp->data->state = ACTIVE;
    }
    tmp = tmp->next;
  }
}
