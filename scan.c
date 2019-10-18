// scan.c
// Copyright (c) Randall Bohn 2019
// BSD 2 clause License

#include <ctype.h>
#include <stdio.h> // temporary
#include "scan.h"

int classify(char ch) {
	if (isdigit(ch)) return NUMBER;
	if (isblank(ch)) return BLANK;
	if (ch == '+') return OPERATOR;
	if (ch == '-') return OPERATOR;
	if (ch == '*') return OPERATOR;
	if (ch == '/') return OPERATOR;
	if (ch == '%') return OPERATOR;
	return OTHER;
}

void scan(char * s)
{
	int n = 0;
	int p = 0;
	parser_state_t pstate = PARSE_OK;
	char ch;
	int current = 0;
	int that;

	while (*(s+n) > 0 && pstate == PARSE_OK) {
		ch = *(s+n);
		that = classify(ch);
		if (current == 0) current = that;
		if (that != current) {
			pstate = accept(current, s+p, n-p);
			current = that;
			p = n;
		}
		n++;
	}
	// set final token
	accept(current, s+p, n-p);
	accept(EOL, s+n, 0);;
}
