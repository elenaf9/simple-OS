#pragma once
enum st_interrupt {
  PERIOD_TIMER,
  ALARM,
  NONE
};

void init_st(void);
enum st_interrupt is_st_interrupt(void);
unsigned int get_crtr(void);
unsigned int get_alms(void);
void set_alms(unsigned int);
void busy_wait(int);
