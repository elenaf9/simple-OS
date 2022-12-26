#ifndef THREADS_H
#define THREADS_H

#include <stddef.h>
struct pcb {
  size_t id;
  size_t pc;
  size_t sp;
};

#endif
