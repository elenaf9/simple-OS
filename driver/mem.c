void mem_write_u32(unsigned int addr, unsigned int val) {
  *(volatile unsigned int *)addr = val;
}

unsigned int mem_is_set(unsigned int addr, unsigned int bit) {
  if ((*(volatile unsigned int *)addr & bit) == bit) {
    return 1;
  } else {
    return 0;
  }
}

unsigned int mem_read_u32(unsigned int addr) {
  return *(volatile unsigned int *)addr;
}