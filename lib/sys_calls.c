#include <threads.h>

int fork(thread_fn _thread_function, int _thread_parameter){
    asm(
        "mov r2, lr"
    );
    asm(
        "swi 2"
    );
}
