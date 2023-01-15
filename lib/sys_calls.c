#include <threads.h>

int fork(){
    asm(
        "mov r0, lr"
    );
    asm(
        "swi 2"
    );
}
