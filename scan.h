// scan.h
// Copyright (c) Randall Bohn 2019
// BSD 2 Clause License

// use char * because cc65 can't do plaid\H\H\H\H\Hinteger arrays
void scan(char *s);

#define BLANK 'B'
#define NUMBER 'N'
#define OPERATOR 'O'
#define OTHER '?'
#define EOL 'Z'

typedef enum {
	PARSE_OK,
	PARSE_BOGUS		/* signals "word not found" */
} parser_state_t;

parser_state_t accept(char token, char *text, int len);
