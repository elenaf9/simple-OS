#ifndef THREADS_H
#define THREADS_H

void init_threading(void);
int spawn_thread(unsigned int);
void switch_thread(void);
void exit_thread(void);

void on_st_alarm(void);
void on_dbgu_rx(char c);
void thread_wait_char(void);
void thread_sleep(unsigned int ms);

#endif
