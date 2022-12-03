#include <mem.h>
#include <stdint.h>
#include <print.h>

#define ST_ADDR 0xFFFFFD00
#define ST_PIMR 0x04
#define ST_SR 0x10
#define ST_IER 0x14
#define ST_IDR 0x18
#define ST_IMR 0x1C

#define SYSTEM_TIMER_INTERRUPT_INTERVAL 32769 // This corresponds to one sec

void set_timer(uint16_t timer) {
  mem_write_u32(ST_ADDR + ST_PIMR, timer);
}

void init_st(void) {
  set_timer(SYSTEM_TIMER_INTERRUPT_INTERVAL);
  mem_write_u32(ST_ADDR + ST_IER, 1 << 0);
  printf("st enabled\n");
}