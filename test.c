#include <stdio.h>
#include "scan.h"

// so many ways to fail
#define NOT 0
#define TOO_MANY_TOKENS 1
#define WRONG_TOKEN 2

#define TOKENSIZE 8
int tokens[TOKENSIZE];
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
		printf("token[%d]=%d\n", x, tokens[x]);
	}
}

int evaluate(char *s, int expected[]) {
	int x;
	reset();
	scan(s, tokens);
	for (x = 0; x < TOKENSIZE; x++) {
		if (tokens[x] == 0) break;
		if (expected[x] != tokens[x]) {
			printf("FAIL [%s]\n", s);
			printf("FAIL expected=%d actual=%d\n", expected[x], tokens[x]);
			fail=WRONG_TOKEN;
		}
	}

	if (fail != NOT) {
		showTokens();
		return 1;
	} else {
		return 0;
	}
}
	
#define TEST(fixture, expectation) cases++; failures += evaluate(fixture, expectation);

int main() {
	int cases = 0;
	int failures = 0;

	//TEST("xyz", (int[]){NUMBER})
	TEST("555", (int[]){NUMBER})
	TEST("100 * 3 / 4", ((int[]){NUMBER, OPERATOR, NUMBER, OPERATOR, NUMBER}))

	printf("cases=%d failures=%d\n", cases, failures);
}