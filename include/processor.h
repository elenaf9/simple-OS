#pragma once

void _switch_cpu_mode(int);
void _init_cpu_stack(int);

void _cpu_disable_interrupt(void);
void _cpu_enable_interrupt(void);

unsigned int _init_idle_thread_stack(int index, void (*)(void));
unsigned int _switch_usr_stack(unsigned int);
