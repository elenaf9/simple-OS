#pragma once

void _switch_cpu_mode(int);
void _init_stack(int);

void _cpu_disable_interrupt(void);
void _cpu_enable_interrupt(void);
