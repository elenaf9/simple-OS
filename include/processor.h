#pragma once

#include "threads.h"

void _switch_cpu_mode(int);
void _init_stack(int);

void _cpu_disable_interrupt(void);
void _cpu_enable_interrupt(void);

unsigned int _init_thread(int index, void (*run)(thread_fn, int), thread_fn,
                          int);
void _switch_thread(volatile unsigned int *, volatile unsigned int *);
