#ifndef SYSCALLS_H
#define SYSCALLS_H

char read(void);
void write(char c);
void end_thread(int thread_id);
void delay_thread(unsigned int timer);
int create_thread(int *thread_function, int thread_parameter);

#endif