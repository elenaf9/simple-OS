#define DBGU 0xfffff200 // Debug Unit

#define DBGU_CR 0x0000  // Debug unit control register
#define RSTRX (1 << 2)  // Reset RX
#define RSTTX (1 << 3)  // Reset TX
#define RXEN (1 << 4)   // Enable RX
#define TXEN (1 << 6)   // Enable TX
#define RSTSTA (1 << 8) // Reset Status Bits

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

static inline void write_u32(unsigned int addr, unsigned int val) {
  *(volatile unsigned int *)addr = val;
}

static inline unsigned int read_u32(unsigned int addr) {
  return *(volatile unsigned int *)addr;
}

static inline unsigned int is_set(unsigned int addr, unsigned int bit) {
  if ((*(volatile unsigned int *)addr & bit) == bit) {
    return 1;
  } else {
    return 0;
  }
}

// Enable the debug unit receiver and transmitter.
void init_dbgu(void) {
  write_u32(DBGU + DBGU_CR, RXEN | TXEN);
}

// Reset the debug unit receiver, transmitter, and status bits.
void reset(void) {
  write_u32(DBGU + DBGU_CR, RSTRX | RSTTX | RSTSTA);
}

// Write a single char to the transmitter.
void write_char(char val) {
  // Wait transmitter to be ready
  while (!is_set(DBGU + DBGU_SR, TXRDY)) {
  }
  // Write value
  write_u32(DBGU + DBGU_THR, val);
//   // Wait for value to be transmitted.
//   while (!is_set(DBGU + DBGU_SR, TXEMPTY)) {
//   }
}

// Read a single char from the receiver.
char read_char(void) {
  // Wait until we received something
  while (!is_set(DBGU + DBGU_SR, RXRDY)) {
  }

  // Read value
  char val = read_u32(DBGU + DBGU_RHR);
  return val;
}
