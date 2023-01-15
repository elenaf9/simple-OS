int fork(){
    asm(
        "mov r0, lr"
        
    );
    asm(
        "swi 1"
    );
}

int exit(){
    asm("swi 2");
}