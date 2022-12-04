#include <mem.h>
#include <stdint.h>
#include <print.h>

#define ST_ADDR 0xFFFFFD00
#define ST_PIMR 0x04

#define ST_SR 0x10 // ST Status Register
#define PITS (1 << 0) // Period Interval Timer Status
#define RTTIN (1 << 2) //  Real-time Timer Increment

#define ST_IER 0x14
#define ST_IDR 0x18
#define ST_IMR 0x1C

#define ST_RTMR 0xC // Real-time Mode Register

#define ST_INTERVAL 0x8000 // = 32769; This corresponds to 1s

void set_timer(uint16_t timer) {
  mem_write_u32(ST_ADDR + ST_PIMR, timer);
}

int is_st_interrupt(void){
  int status  = mem_read_u32(ST_ADDR + ST_SR);
  return status & PITS;
}

void sleep(int ms) {
  int i = 0;
  while (i < ms) {
    while (!mem_is_set(ST_ADDR + ST_SR, RTTIN)) {};
    i++;
  }
}

void init_st(void) {
  set_timer(ST_INTERVAL);

  // Enable timer interrupt
  mem_write_u32(ST_ADDR + ST_IER, 1 << 0);

  // init rt timer to increment its counter every ~1ms
  mem_write_u32(ST_ADDR + ST_RTMR, (ST_INTERVAL / 10000 ));

  printf("st enabled\n");
}
