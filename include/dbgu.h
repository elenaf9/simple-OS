#ifndef DBGU_H
#define DBGU_H

void write_char(char val);
char read_char(void);
void init_dbgu(void);
void reset(void);
unsigned int read_u32(unsigned int addr);

#endif
