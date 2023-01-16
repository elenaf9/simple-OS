
#include <irq.h>
#include <mem.h>
#include <print.h>
#include <processor.h>

#define AIC_BASE_ADDR 0xFFFFF000
#define AIC_SMR1 0X04  // Source Mode Register 1
#define AIC_SVR1 0X84  // Source Vector Register 1
#define AIC_IVR1 0x100 // Interrupt Vector Register
#define AIC_OFFSET_INTERRUPT_MASK_REG 0x110

#define AIC_IECR 0x120 // Interrupt Enable Command Register
#define SYS (1 << 1)   // Interrupt Source 1

#define AIC_EOICR 0x130 // End of Interrupt Command Register

#define IVT_IRQ 0x18

void init_aic(void) {
  // Enable Interrupt source  1  (System: sys clock, dbgu, ..)
  mem_write_u32(AIC_BASE_ADDR + AIC_IECR, SYS);

  // Handler address for IRQ
  int irq_pointer = (volatile unsigned int)_irq;
  mem_write_u32(AIC_BASE_ADDR + AIC_SVR1, irq_pointer);

  // Enable interrupt in current processor mode
  _cpu_enable_interrupt();

  printf("aic enabled\n");
}

void end_of_interrupt(void) { mem_write_u32(AIC_BASE_ADDR + AIC_EOICR, 1); }
