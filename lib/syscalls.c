
#define READ_ID 1
#define WRITE_ID 2
#define CREAT_THREAD_ID 3
#define END_THREAD_ID 4
#define DELAY_THREAD_ID 5

extern int _swi(int num, int arg1, int arg2, int arg3);

char read() { (char)_swi(READ_ID, 0, 0, 0); }

void write(char c) { _swi(WRITE_ID, c, 0, 0); }

void create_thread(int thread_function, int thread_parameter) {
  _swi(CREAT_THREAD_ID, thread_function, thread_parameter, 0);
}

void end_thread(int thread_id) { _swi(END_THREAD_ID, thread_id, 0, 0); }

void delay_thread(unsigned int timer) { _swi(DELAY_THREAD_ID, timer, 0, 0); }