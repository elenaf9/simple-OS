
#include <mem.h>
#include <print.h>

#define AIC_BASE_ADDR 0xFFFFF000
#define AIC_OFFSET_SOURCE_MODE1 0X04
#define AIC_SVR 0X84
#define AIC_IVR 0x100
#define AIC_OFFSET_INTERRUPT_MASK_REG 0x110
#define AIC_IECR 0x120
#define AIC_EOICR 0x130

extern void *irq;

void test() {
  printf("Test\n");
  mem_write_u32(AIC_BASE_ADDR + AIC_EOICR, 42);
  while (1) {
  };
}

void init_aic(void) {

  mem_write_u32(AIC_BASE_ADDR + AIC_IECR, 1 << 1);

  mem_write_u32(AIC_BASE_ADDR + AIC_SVR, 0x20);

  printf("aic enabled\n");
}
