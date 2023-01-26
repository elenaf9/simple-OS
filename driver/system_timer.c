#include <mem.h>
#include <print.h>
#include <stdint.h>

#define ST_ADDR 0xFFFFFD00
#define ST_PIMR 0x04

#define ST_SR 0x10     // ST Status Register
#define PITS (1 << 0)  // Period Interval Timer Status
#define RTTIN (1 << 2) //  Real-time Timer Increment
#define ALMS (1 << 3) //  Alarm status

#define ST_IER 0x14
#define ST_IDR 0x18
#define ST_IMR 0x1C


#define ST_RTMR 0xC // Real-time Mode Register

#define ST_RTAR 0x20 // Real-time Alarm Register
#define ST_CRTR 0x24 // Current Real-time Register

#define ST_INTERVAL 0x8000/1000 // = 32768:1000; This corresponds to 1ms

enum st_interrupt {
  PERIOD_TIMER,
  ALARM,
  NONE
};

enum st_interrupt is_st_interrupt(void) {
  int status = mem_read_u32(ST_ADDR + ST_SR);
  if (status & PITS) {
    return PERIOD_TIMER;
  }
  if (status & ALMS) {
    return ALARM;
  }
  return NONE;
}

unsigned int get_crtr(void) {
  return mem_read_u32(ST_ADDR + ST_CRTR);
}

unsigned int get_alms(void) {
  return mem_read_u32(ST_ADDR + ST_RTAR);
}

void set_alms(unsigned int time) {
  return mem_write_u32(ST_ADDR + ST_RTAR, time);
}

void busy_wait(unsigned int ms) {
  unsigned int i = 0;
  while (i < ms) {
    while (!mem_is_set(ST_ADDR + ST_SR, RTTIN)) {
    };
    i++;
  }
}


void init_st(void) {
  // Init periodic timer to trigger every ~10ms.
  mem_write_u32(ST_ADDR + ST_PIMR, ST_INTERVAL*10);

  // Init rt timer to increment its counter every ~1ms.
  mem_write_u32(ST_ADDR + ST_RTMR, ST_INTERVAL);

  // Enable timer interrupt
  mem_write_u32(ST_ADDR + ST_IER, (PITS|ALMS));
}
