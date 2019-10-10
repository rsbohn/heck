// scan.h
// Copyright (c) Randall Bohn 2019
// BSD 2 Clause License

// use char * because cc65 can't do plaid\H\H\H\H\Hinteger arrays
char *scan(char *s, char *tokens);

#define NUMBER 'N'
#define OPERATOR 'O'
#define OTHER '?'
#define EOL 'Z'

// operators
#define O_NONE 0
#define O_ADD 1
#define O_SUB 2
#define O_MUL 3
#define O_DIV 4
#define O_MOD 5

