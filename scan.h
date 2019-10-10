// scan.h
// Copyright (c) Randall Bohn 2019
// BSD 2 Clause License

int *scan(char *s, int tokens[]);

#define NUMBER 80
#define OPERATOR 81
#define OTHER 82

// operators
#define O_NONE 0
#define O_ADD 1
#define O_SUB 2
#define O_MUL 3
#define O_DIV 4
#define O_MOD 5

