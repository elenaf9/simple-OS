#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

void handle_data_abort(void);
void handle_prefetched_abort(void);
int handle_software_interrupt(int num, int arg1, int arg2, int arg3);
void handle_undefined_instruction(int);
void handle_irq(void);
void handle_fiq(void);
void handle_reset(void);

#endif
