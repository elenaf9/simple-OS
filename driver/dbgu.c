#include <mem.h>
#include <print.h>

#define DBGU 0xfffff200 // Debug Unit

#define DBGU_CR 0x0000  // Debug unit control register
#define RSTRX (1 << 2)  // Reset RX
#define RSTTX (1 << 3)  // Reset TX
#define RXEN (1 << 4)   // Enable RX
#define TXEN (1 << 6)   // Enable TX
#define RSTSTA (1 << 8) // Reset Status Bits

#define DBGU_IER 0x0008 // Debug Unit Interrupt Enable Register
#define IER_RXRDY (1 << 0) // Enable RXRDY Interrupt

#define DBGU_SR 0x0014   // Debug unit status register
#define RXRDY (1 << 0)   // Receiver Ready: Complete char received
#define TXRDY (1 << 1)   // Transmitter Ready (To receive the next char)
#define TXEMPTY (1 << 9) // Transmitter Emtpy (Send done)

// Read errors
#define OVRE                                                                   \
  (1 << 5) // Overrun error: DEBUG_RHR was overwritten before old value was read
#define FRAME (1 << 6) // Framing error
#define PARE (1 << 7)  // Parity error

#define DBGU_RHR 0x0018 // Receive holding register
#define DBGU_THR 0x001C // Transmit holding register

// Enable the debug unit receiver and transmitter.
void init_dbgu(void) {
  mem_write_u32(DBGU + DBGU_CR, RXEN | TXEN);
  // Enable interrupt on rx
  mem_write_u32(DBGU + DBGU_IER, IER_RXRDY);
}

// Reset the debug unit receiver, transmitter, and status bits.
void reset(void) {
  mem_write_u32(DBGU + DBGU_CR, RSTRX | RSTTX | RSTSTA);
}

// Write a single char to the transmitter.
void write_char(char val) {
  // Wait transmitter to be ready
  while (!mem_is_set(DBGU + DBGU_SR, TXRDY)) {
  }
  // Write value
  mem_write_u32(DBGU + DBGU_THR, val);

  // Wait for value to be transmitted.
  while (!mem_is_set(DBGU + DBGU_SR, TXEMPTY)) {
  }
}

int is_dbgu_rx_ready(void) {
  return mem_is_set(DBGU + DBGU_SR, RXRDY);
}

char get_char(void) {
  // Read value
  char val = mem_read_u32(DBGU + DBGU_RHR);
  return val;
}
