#ifndef THREADS_H
#define THREADS_H

typedef void (*thread_fn)(int);

void init_threading(void);
int spawn_thread(unsigned int);
void thread_switch(void);

#endif
