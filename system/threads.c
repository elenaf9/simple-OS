#include <stddef.h>
#include <system_timer.h>
#include <mem.h>
#include <print.h>
#include <processor.h>
#include <tests.h>
#include <threads.h>

#define NUM_THREADS 16
#define STACK_SIZE 0x1000 // 4kB
#define THREADS_STACK_BOTTOM 0x23000000 

// Since the stack grows towards 0 we can place the TCBS at the same address,
// i.e. behind the bottom of the first thread's stack
#define TCBS_BASE THREADS_STACK_BOTTOM

enum thread_state { READY, RUNNING, WAITING, INACTIVE};

struct list_elem {
  struct list_elem *prev;
  struct list_elem *next;
  struct tcb *data;
};

struct resources {
  // > 0 if the thread is waiting for a system timer time
  // 0 if not blocked on system timer
  unsigned int st_time;
  // Whether the thread is waiting for a char from dbgu.
  _Bool is_waiting_char;
};

struct tcb {
  unsigned int id;
  unsigned int sp;
  enum thread_state state;
  struct list_elem element;
  struct resources waiting_for;
};

static struct tcb *const tcbs = (struct tcb *)TCBS_BASE;
// Queue of active threads. The idle thread is never enqueued!
// NULL in case no thread is active, i.e. idle thread is running.
struct list_elem *runqueue = NULL;



// Idle thread that runs if no other thread is active.
static void idle_thread() {
  while (1) {}
}

static void run_idle_thread(struct tcb *prev) {
  struct tcb *idle_thread = &tcbs[0];
  unsigned int prev_sp = _switch_usr_stack(idle_thread->sp);
  if (prev) {
    prev->sp = prev_sp;
  }
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
    tcbs[i].waiting_for.is_waiting_char = 0;
    tcbs[i].waiting_for.st_time = 0;
  }

  struct tcb *idle_tcb = &tcbs[0];
  // Initialized a stack for this thread so that the correct context is loaded on 
  // thread switch.
  idle_tcb->sp = _init_idle_thread_stack(THREADS_STACK_BOTTOM, (unsigned int)&idle_thread);
}

// Copy the stack of the parent thread to the child.
static unsigned int copy_thread_stack(unsigned int child_stack_bottom, unsigned int parent_stack_bottom, unsigned int parent_sp) {
  int parent_used_stack_size = parent_stack_bottom - parent_sp;

  unsigned int child_sp = child_stack_bottom-parent_used_stack_size;
  
  unsigned int src_addr = parent_stack_bottom;
  unsigned int dst_addr = child_stack_bottom;
  while (dst_addr > child_sp) {
    dst_addr -= 4;
    src_addr -= 4;
    mem_write_u32(dst_addr, mem_read_u32(src_addr));
  }
  return child_sp;
}

// Switch to the next thread in the runqueue if there are more than one threads
// active.
void switch_thread(void) {

  struct list_elem *prev = runqueue;

  // Remove waiting or exited thread from runqueue.
  if (prev && (prev->data->state == INACTIVE || prev->data->state == WAITING)) {
    if (prev->next == prev) {
      runqueue = NULL;
    } else {
      // Remove finished thread from runqueue.
      runqueue = prev->prev;
      runqueue->next = prev->next;
      runqueue->next->prev = runqueue;
    }
  }

  if (!runqueue) {
    struct tcb *prev_data = NULL;
    if (prev) {
      prev_data = prev->data;
    }
    run_idle_thread(prev_data);
    return;
  }

  runqueue = runqueue->next;
  if (runqueue->data->state == RUNNING) {
    // Only single thread active -  no thread switch needed.
    return;
  }

  if (prev->data->state == RUNNING) {
    prev->data->state = READY;
  }

  runqueue->data->state = RUNNING;

  unsigned int prev_sp = _switch_usr_stack(runqueue->data->sp);
  if (prev != runqueue) {
    prev->data->sp = prev_sp;
  }
}

static void enqueue_thread(struct list_elem* elem) {
  elem->data->state = READY;
  if (!runqueue) {
    // Insert as only element in runqueue.
    elem->next = elem;
    elem->prev = elem;
    runqueue = elem;
  } else {
    // Enqueue as next thread.
    elem->prev = runqueue;
    elem->next = runqueue->next;
    runqueue->next->prev = elem;
    runqueue->next = elem;
  }
}

int spawn_thread(unsigned int parent_sp) {
  // Insert at a free slot in the TCBs array.
  _Bool is_space = 0;
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

  int child_stack_bottom = THREADS_STACK_BOTTOM - (child->id*STACK_SIZE);
  int parent_stack_bottom;

  // Check if parent_sp is in the memory allocated for our threads.
  // If it's not it means that our parent is not a thread, but instead the main
  // function, in which case the processor system mode stack needs to be cloned.
  if (
    runqueue &&
    parent_sp <= THREADS_STACK_BOTTOM && 
    parent_sp >= THREADS_STACK_BOTTOM - (NUM_THREADS+1)*STACK_SIZE) 
  {
    parent_stack_bottom = THREADS_STACK_BOTTOM - (runqueue->data->id*STACK_SIZE);
  } else {
    // Hardcoded system mode stack bottom.
    parent_stack_bottom = 0x24000000;
  }

  child->sp = copy_thread_stack(child_stack_bottom, parent_stack_bottom, parent_sp);
  
  enqueue_thread(&child->element);

  return i;
}

void exit_thread(void) {
  if (!runqueue) {
    printf("Error: `exit` may only be called within a thread context!\n");
    return;
  }
  struct list_elem *self = runqueue;
  self->data->state = INACTIVE;
  switch_thread();
}

void thread_sleep(unsigned int ms) {
  if (!runqueue) {
    printf("Error: `sleep` may only be called within a thread context!\n");
    return;
  }
  unsigned int time = get_crtr();
  unsigned int until = time + ms;
  struct list_elem *thread = runqueue;
  thread->data->waiting_for.st_time = until;
  thread->data->state = WAITING;
  unsigned int alms = get_alms();
  if (!alms || alms > until || alms < time) {
    set_alms(until);
  }
  switch_thread();
}

void thread_wait_char(void) {
  if (!runqueue) {
    printf("Error: `read` may only be called within a thread context!\n");
    return;
  }
  runqueue->data->waiting_for.is_waiting_char = 1;
  runqueue->data->state = WAITING;
  switch_thread();
}

void on_dbgu_rx(char c) {
  // Unblock first thread that is waiting for a char.
  // Note: this is unfair since we are always search the tcb array starting from the front, i.e.
  // if multiple threads are waiting for a char the one with the lowest index wins.
  int i;
  for(i = 1; i < (NUM_THREADS + 1); i++) {
    struct tcb *curr = &tcbs[i];
    if (curr->state == WAITING && curr->waiting_for.is_waiting_char) {

      // The newest item pushed to the thread's stack was r0.
      // Overwrite it with the received char so it is returned from the
      // `read` sys call.
      *(unsigned char *)curr->sp = c;
      curr->waiting_for.is_waiting_char = 0;

      _Bool was_queue_empty = runqueue == 0;
      enqueue_thread(&curr->element);
      if (was_queue_empty) {
        switch_thread();
      }
      break;
    }
  }
}

void on_st_alarm(void) {
  unsigned int time = get_crtr();

  unsigned int soonest = 0;
  unsigned int unblocked = 0;
  _Bool was_queue_empty = runqueue == 0;

  int i;
  for(i=1; i<NUM_THREADS+1; i++) {

    struct tcb *curr = &tcbs[i];
    unsigned int waiting_st = curr->waiting_for.st_time;

    if (curr->state != WAITING || !waiting_st) {
      continue;
    }

    if (waiting_st > time) {
      // Not yet read, but could be used to set next alarm
      // if its the soonest.
      if (!soonest || waiting_st < soonest) {
        soonest = waiting_st;
      }
      continue;
    }

    curr->waiting_for.st_time = 0;
    enqueue_thread(&curr->element);
    unblocked++;
  }

  if (soonest) {
    set_alms(soonest);
  }
  
  if (was_queue_empty && unblocked) {
    switch_thread();
  }
}