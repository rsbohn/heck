// scan.c
// Copyright (c) Randall Bohn 2019
// BSD 2 clause License

#include <ctype.h>
#include <stdio.h> // temporary
#include "scan.h"

int classify(char ch) {
	if (isdigit(ch)) return NUMBER;
	if (ch == '+') return OPERATOR;
	if (ch == '-') return OPERATOR;
	if (ch == '*') return OPERATOR;
	if (ch == '/') return OPERATOR;
	if (ch == '%') return OPERATOR;
	return OTHER;
}

int *scan(char *s, int tokens[])
{
	int n = 0;
	int p = 0;
	char ch;
	int current = 0;
	int that;

	while (*(s+n) > 0) {
		ch = *(s+(n++));
		if (ch == ' ') continue;
		if (ch == '\t') continue;
		that = classify(ch);
		//printf("(%d,%d) %c\n", current, that, ch);
		if (current == 0) current = that;
		if (that != current) {
			tokens[p++] = current;
			current = that;
		}
	}
	// set final token
	tokens[p] = current;
	return tokens;
}