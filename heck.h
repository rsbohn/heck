#define BUFFSIZE 32

int reader();
void evaluate();
void print();
int select(char op);

// lex states
#define GEOF 0
#define IDLE 1
#define BFULL 2
#define RESOLVE 3


