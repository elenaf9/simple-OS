
#include <dbgu.h>
#include <mem.h>
#include <print.h>

#define AIC_BASE_ADDR 0xFFFFF000
#define AIC_SM1 0X04
#define AIC_SVR1 0X84
#define AIC_IVR 0x100
#define AIC_OFFSET_INTERRUPT_MASK_REG 0x110
#define AIC_IECR 0x120
#define AIC_EOICR 0x130
#define AIC_ICCR 0x128

extern void *irq;

void aknowledge_irq(void) { mem_write_u32(AIC_BASE_ADDR + AIC_EOICR, 1 << 1); }
void clear_interrupt(void) { mem_write_u32(AIC_BASE_ADDR + AIC_ICCR, 1 << 1); }
void read_irq() { read_u32(AIC_BASE_ADDR + AIC_IVR); }

void init_aic(void) {

  mem_write_u32(AIC_BASE_ADDR + AIC_IECR, 1 << 1);
  mem_write_u32(AIC_BASE_ADDR + AIC_SM1, 0b111);
  mem_write_u32(AIC_BASE_ADDR + AIC_SVR1, 0x78);

  printf("aic enabled\n");
}
