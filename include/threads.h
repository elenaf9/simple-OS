#ifndef THREADS_H
#define THREADS_H

#include <stddef.h>
struct pcb {
  size_t id;
  size_t pc;
  size_t sp;
};

void init_idle_thread(void);
void idle_thread(void);
void thread_switch(void);

#endif
