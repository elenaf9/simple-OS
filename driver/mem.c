#include <mem.h>

void mem_write_u32(unsigned int addr, unsigned int val) {
  *(volatile unsigned int *)addr = val;
}