#define BUFFSIZE 32

int readToken();
void evaluate();
void print();
void accept(int token);
int select(char op);

// lex states/token types
#define GEOF 0
#define IDLE 1
#define BFULL 2
#define NUMBER 3
#define OPERATOR 4
#define RESOLVE 5
#define OTHER 6

// operators
#define O_NONE 0
#define O_ADD 1
#define O_SUB 2
#define O_MUL 3
#define O_DIV 4
#define O_MOD 5

