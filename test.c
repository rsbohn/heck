#include <stdio.h>
#include "scan.h"

// so many ways to fail
#define NOT 0
#define TOO_MANY_TOKENS 1
#define WRONG_TOKEN 2

#define TOKENSIZE 8
char tokens[TOKENSIZE];
int tptr = 0;
int fail = NOT;

void accept(int token) {
	tokens[tptr++] = token;
	if (tptr >= TOKENSIZE) fail=TOO_MANY_TOKENS;
}

void reset() {
	fail = NOT;
	for (tptr = TOKENSIZE-1; tptr > 0; tptr--) {
		tokens[tptr] = 0;
	}
}
void showTokens() {
	int x;
	for (x = 0; x < TOKENSIZE; x++) {
		if (tokens[x] < 1) break;
		printf("token[%d]=%c\n", x, tokens[x]);
	}
}

int cases = 0;
int failures = 0;

int evaluate(char *s, char expected[]) {
	int x;
	cases++;
	reset();
	scan(s, tokens);
	for (x = 0; x < TOKENSIZE; x++) {
		if (tokens[x] == 0) break;
		if (expected[x] != tokens[x]) {
			printf("FAIL [%s]\n", s);
			printf("FAIL expected=%c actual=%c\n", expected[x], tokens[x]);
			fail=WRONG_TOKEN;
		}
	}

	if (fail != NOT) {
		failures++;
		showTokens();
		return 1;
	} else {
		return 0;
	}
}
	
void report() 
{
	if (failures == 0) printf("GREEN ");
	else printf("RED ");
	printf("cases=%d failures=%d\n", cases, failures);
}

int main() {
	evaluate("555", "NZ");

	evaluate("100 * 3 / 4", "NONONZ");

	report();
	evaluate("xyz", "NZ");
	report();
	return failures;
}
