int fork(void){
    asm("mov r0, lr;" 
        "swi 1"
    );
}

int exit(void) {
    asm("swi 2");
}