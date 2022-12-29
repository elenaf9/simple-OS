#include "system_timer.h"
#include <mem.h>
#include <print.h>
#include <processor.h>
#include <threads.h>

// Place TCBs somewhere in RAM
#define TCBS_BASE 0x22000000

#define NUM_THREADS 16
#define THREAD_STACK_SIZE 4096

enum thread_state { CREATED, ACTIVE, STOPPED };

struct list_elem {
  volatile struct list_elem *prev;
  volatile struct list_elem *next;
  volatile struct tcb *data;
};

struct tcb {
  unsigned int id;
  unsigned int sp;
  enum thread_state state;
  struct list_elem element;
};

static volatile struct tcb *const tcbs = (struct tcb *)TCBS_BASE;

volatile struct list_elem *runqueue;

// First parameter will be set to 0 from OS.
// If it holds a value != 0 passing the parameters failed and
// the thread can not trust any following parameters.
typedef void (*thread_fn)(int);

static void run_thread(thread_fn thread_fn, int thread_parameter) {

  runqueue->data->state = ACTIVE;

  // Run thread
  thread_fn(thread_parameter);

  runqueue->data->state = STOPPED;
  runqueue->next->prev = runqueue->prev;
  runqueue->prev->next = runqueue->next;

  for (;;)
    ;
}

// Create new TCB for thread.
// Note: tcb.list_elem.{prev,next} is not set.
static void init_tcb(unsigned int index, thread_fn thread_fn,
                     int thread_parameter) {

  volatile struct tcb *new_tcb = &tcbs[index];
  new_tcb->sp = _init_thread(index, &run_thread, thread_fn, thread_parameter);
  new_tcb->id = index;
  new_tcb->state = CREATED;
  new_tcb->element.data = new_tcb;

  return;
}

static void idle_thread(int error) {
  if (error) {
    // Something went wrong with parameter list.
    printf("PANIC: wrong parameter!!!");
    asm("udf");
  }
  while (1) {
    printf("Idle\n");
    busy_wait(1000);
  }
}

void init_threading(void) {

  // Idle thread
  init_tcb(0, &idle_thread, 0);

  // init idle thread
  volatile struct tcb *new_tcb = &tcbs[0];
  new_tcb->element.next = &new_tcb->element;
  new_tcb->element.prev = &new_tcb->element;
  runqueue = &new_tcb->element;

  // Init state of empty tcbs
  int i;
  for (i = 1; i < NUM_THREADS + 1; i++) {
    tcbs[i].state = STOPPED;
  }
}

int spawn_thread(thread_fn thread_fn, int thread_parameter) {
  // search for free tcb
  int is_space = 0;
  int i;
  for (i = 1; i < NUM_THREADS + 1; i++) {
    if (tcbs[i].state == STOPPED) {
      is_space = 1;
      break;
    }
  }

  if (!is_space) {
    return 1;
  }

  init_tcb(i, thread_fn, thread_parameter);

  // Insert as next thread to run_queue
  volatile struct tcb *new_tcb = &tcbs[i];


  // Insert as next thread except for threads that themselves
  // did not run yet.
  volatile struct list_elem *insert_after = runqueue;
  while(insert_after->next != runqueue) {
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

  return 0;
}

void thread_switch(void) {
  if (runqueue->next == runqueue && runqueue->data->state != CREATED) {
    printf("Only idle thread active -  no thread switch needed.\n");
    return;
  }

  runqueue = runqueue->next;

  _switch_thread(&runqueue->prev->data->sp, &runqueue->data->sp);
}
