#include <stdarg.h>
#include <dbgu.h>
#include <stddef.h>
#include <system_timer.h>
#include <sys_calls.h>

// Print integer as hexadecimal
static void print_as_hex(unsigned int val) {

  char hex[50];

  int i = 0;

  write('0');
  write('x');

  if (val == 0) {
    write('0');
    return;
  }
  while (val > 0) {
    int tmp = val % 16;

    if (tmp < 10) {
      hex[i++] = tmp + 48;
    } else {
      hex[i++] = tmp + 55;
    }

    val = val / 16;
  }

  while (i >= 0) {
    write(hex[i]);
    i--;
  }
  return;
}

// Print a formatted string.
__attribute__((format(printf, 1, 2))) int printf(char *fmt, ...) {
  // Variable input list
  va_list ap;

  int ret = 0;

  // Store previous character to identify a '%'.
  char prev = ' ';

  char c;
  char *s;
  int x;
  size_t *p;

  va_start(ap, fmt);
  
  while (*fmt) {
    char curr = *fmt++;
    if (prev == '%') {
      switch (curr) {
      case 'c':
        c = (char)va_arg(ap, int);
        write(c);
        break;
      case 's':
        s = va_arg(ap, char *);
        while (*s) {
          write(*s++);
        }
        break;
      case 'p':
        p = va_arg(ap, void *);
        print_as_hex((size_t)p);
        break;
      case 'x':
        x = va_arg(ap, int);
        print_as_hex(x);
        break;
      default:
        ret = 1;
        goto finish;
      }
    } else if (!(curr == '%')) {
      write(curr);
    }
    prev = curr;
  }
finish:
  va_end(ap);
  return ret;
}

// Print char for an ascii-code *n* times with a short delay, 
// with *n* being the ascii code.
void periodically_print_char(char c) {
  int i;
  for (i = 0; i < (int) c; i++) {
    printf("%c", c);
    if (c >= 'a') {
      sleep(100); // sleep 100ms
    } else {
      busy_wait(100); // block 100ms
    }
  }
}
