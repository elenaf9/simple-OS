#ifndef THREADS_H
#define THREADS_H

void init_threading(void);
int spawn_thread(unsigned int);
void switch_thread(void);

#endif
