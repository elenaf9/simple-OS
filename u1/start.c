#include "print.h"

__attribute__((naked, section(".init")))
void _start(void) {
    while (1) {
        unsigned char val = read_char();
        write_char(val);
        write_char('\n');
    }
    reset();
}