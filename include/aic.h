#ifndef AIC_H
#define AIC_H
void aknowledge_irq(void);
void clear_interrupt(void);
void init_aic(void);
int read_irq(void);
#endif