int fork(void){
    asm("mov r0, lr");
    asm("swi 1");
}

int exit(void) {
    asm("swi 2");
}

void sleep(unsigned int ms) {
    asm("swi 3");
}

char read(void) {
    asm("swi 4"); 
}

void write(char c) {
    asm("swi 5");
}
