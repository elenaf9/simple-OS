#define DBGU 0xfffff200 // Debug Unit

#define DBGU_CR 0x00 // Debug unit control register
#define RSTRX (1 << 2) // Reset RX
#define RSTTX (1 << 3) // Reset TX
#define RXEN (1 << 4) // Enable RX
#define TXEN (1 << 6) // Enable TX
#define RSTSTA (1 << 8) // Reset Status Bits

#define DBGU_SR 0x0014 // Debug unit status register
#define RXRDY (1 << 0) // Receiver Ready: Complete char received
#define TXRDY (1 << 1) // Transmitter Ready (To receive the next char) 
#define TXEMPTY ( 1 << 9)// Transmitter Emtpy (Send done)

// Read errors
#define OVRE (1 << 5) // Overrun error: DEBUG_RHR was overwritten before old value was read
#define FRAME (1 << 6) // Framing error
#define PARE (1 << 7) // Parity error

#define DBGU_RHR 0x0018 // Receive holding register
#define DBGU_THR 0x001C // Transmit holding register

static inline void write_u32 (unsigned int addr, unsigned int val) {
	*(volatile unsigned int *)addr = val;
}

static inline unsigned int read_u32 (unsigned int addr) {
	return *(volatile unsigned int *)addr;
}

static inline void set_bit (unsigned int addr, unsigned int val) {
	*(volatile unsigned int *)addr |= val;
}

static inline void remove_bit (unsigned int addr, unsigned int val) {
	*(volatile unsigned int *)addr &= ~val;
}

static inline unsigned int is_set (unsigned int addr, unsigned int bit) {
    if ((*(volatile unsigned int *)addr & bit) == bit) {
      return 1;
    } else {
      return 0;
    }
}

// Enable receiver and transmitter
void init_dbgu (void) {
  set_bit (DBGU + DBGU_CR, RXEN);
  set_bit (DBGU + DBGU_CR, TXEN);
}
 
// Reset receiver, transmitter, and status bits
void reset (void) {
  set_bit (DBGU + DBGU_CR, RSTRX);
  set_bit (DBGU + DBGU_CR, RSTTX);
  set_bit (DBGU + DBGU_CR, RSTSTA);
}

// Write a single char to the transmitter.
void write_char(unsigned char val) {
  // Wait transmitter to be ready
  while (!is_set (DBGU + DBGU_SR, TXRDY)) {
  }
  // Write value
  write_u32 (DBGU + DBGU_THR, val);
  // Wait for value to be transmitted.
  while (!is_set (DBGU + DBGU_SR, TXEMPTY)) {
  }
}

// Read a single char from the receiver.
unsigned char read_char(void) {
  // Wait until we received something
  while (!is_set (DBGU + DBGU_SR, RXRDY)) {
  }
  // Read value
  unsigned char val = read_u32 (DBGU + DBGU_RHR);
  return val;

  // // Check for errors
  // if (is_set (DBGU + DBGU_SR, OVRE)) {
  //   return OVRE;
  // }
  // if (is_set (DBGU + DBGU_SR, FRAME)) {
  //   return FRAME;
  // }
  // if (is_set (DBGU + DBGU_SR, PARE)) {
  //   return PARE;
  // }
  // return 0;
}
