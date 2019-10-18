#define BUFFSIZE 48

int reader();
void print();
int select(char op);

// lex states
#define GEOF 0
#define IDLE 1
#define BFULL 2
#define RESOLVE 3


