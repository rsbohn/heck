// dict.h
// Copyright (c) Randall Bohn 2019
// BSD 2 Clause License

int locate(char *word);
int xt_call(int entry);
void push(long v);
long top();
int evaluate(int operator);

// operators
#define O_NONE 0
#define O_ADD 1
#define O_SUB 2
#define O_MUL 3
#define O_DIV 4
#define O_MOD 5

