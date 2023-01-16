#ifndef PRINT_H
#define PRINT_H

__attribute__((format(printf, 1, 2))) int printf(char *fmt, ...);

void periodically_print_char(char c);

#endif
