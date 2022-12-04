#ifndef DBGU_H
#define DBGU_H

void write_char(char val);
int is_dbgu_rx_ready(void);
char get_char(void);
char read_char(void);
void init_dbgu(void);
void reset(void);

#endif
