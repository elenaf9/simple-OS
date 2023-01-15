#ifndef THREADS_H
#define THREADS_H

typedef void (*thread_fn)(int);

void init_threading(void);
int spawn_thread(unsigned int);
void thread_switch(void);
void change_thread_wait();

void despawn_thread(int thread_id);
void count_delay();

void check_wait();
void thread_wait();

#endif
