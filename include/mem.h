#pragma once

void remap_mc(void);

void mem_write_u32(unsigned int addr, unsigned int val);
unsigned int mem_is_set(unsigned int addr, unsigned int bit);
unsigned int mem_read_u32(unsigned int addr);
void memcpy(unsigned int dst_addr, unsigned int src_addr, unsigned int size);
