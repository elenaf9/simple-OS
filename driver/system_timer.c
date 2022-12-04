#include <mem.h>
#include <stdint.h>
#include <system_timer.h>

#define ST_ADDR 0xFFFFFD00
#define ST_PIMR 0x04
#define ST_SR 0x10
#define ST_IER 0x14
#define ST_IDR 0x18
#define ST_IMR 0x1C

#define SYSTEM_TIMER_INTERRUPT_INTERVAL                                        \
  0xFFFF // 32768 This coresponds to one sec

void set_timer(uint16_t timer) { mem_write_u32(ST_ADDR + ST_PIMR, timer); }

void init_st(void) {
  set_timer(SYSTEM_TIMER_INTERRUPT_INTERVAL);
  mem_write_u32(ST_ADDR + ST_IER, 1 << 0);
  printf("st enabled\n");
}

void read_pit(void) { mem_read_u32(ST_ADDR + ST_SR); }