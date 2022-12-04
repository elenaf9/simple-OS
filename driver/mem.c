#define USER_I 0xFFFFFF00 // User Interface
#define MC_RCR (1 << 0) // Memory Control Remap Control Register -  Write-only

// Remap memory controller.
// Causes the Internal RAM to be accessed through the Internal Memory Area 0.
void remap_mc(void) { 
  *(volatile unsigned int *)USER_I = MC_RCR;
}

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
