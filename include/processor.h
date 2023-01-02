#pragma once

#include "threads.h"

void _switch_cpu_mode(int);
void _init_cpu_stack(int);

void _cpu_disable_interrupt(void);
void _cpu_enable_interrupt(void);

unsigned int _init_thread_stack(int index, void (*run)(thread_fn, int), thread_fn,
                          int);
void _switch_thread(volatile unsigned int *, volatile unsigned int *);
