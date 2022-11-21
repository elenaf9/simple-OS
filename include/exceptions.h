#ifndef REMAP_H
#define REMAP_H

void remap_mc(void);

void handle_data_abort(void);
void handle_prefetched_abort(void);
void handle_software_interrupt(void);
void handle_undefined_instruction(int);
void handle_irq(void);
void handle_fiq(void);
void handle_reset(void);

void disable_interrupt(void);
void enable_interrupt(void);

#endif
