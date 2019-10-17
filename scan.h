// scan.h
// Copyright (c) Randall Bohn 2019
// BSD 2 Clause License

// use char * because cc65 can't do plaid\H\H\H\H\Hinteger arrays
void scan(char *s);
void accept(char token, char *text, int len);

#define BLANK 'B'
#define NUMBER 'N'
#define OPERATOR 'O'
#define OTHER '?'
#define EOL 'Z'

