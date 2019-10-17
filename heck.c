#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __CX16__
	#include <conio.h>
	#include "graphics.h"
#endif

#include <ctype.h>

#include "heck.h"
#include "scan.h"
#include "dict.h" /* the function dictionary */

char buffer[BUFFSIZE];
int dot = 0;
int prevtoken = IDLE;
int operator = O_NONE;

#define STACKSIZE 8
long stack[STACKSIZE];
int sp = 0;
#define PROMPT "heck: "

void dup()
{
	if (sp < 1) return;
	stack[sp]=stack[sp-1];
	sp++;
}

int main()
{
	int last;
#ifdef __CX16__
	big_pixels();
	he_logo();
#endif

	printf(PROMPT);
	do {
		last = reader();
		if (last == RESOLVE) {
			evaluate();
			printf(PROMPT);
		}
	} while (last != GEOF);
	return 0;
}

#define CTRLD 0x04
#define CTRLH 0x08
int reader() 
{
	char ch;
	int x;

	if (dot >= BUFFSIZE) return BFULL;
	if (!kbhit()) return IDLE;
	ch=cgetc();
	if (ch==CTRLD) {
		return GEOF;
	}
	if (ch==CTRLH) {
		dot--;
		if (dot < 0) dot = 0;
		buffer[dot]=0;
		return IDLE;
	}
	if (ch == '=') {
		scan(buffer);
		dot = 0;
		buffer[dot]=0;
		return RESOLVE;
	}
	if (ch==0x20 || ch==0x0D) {
		scan(buffer);
		dot = 0;
		buffer[dot]=0;
		return RESOLVE;
	}
	if (ch == '.') {
		for (x = sp-1; x >= 0; x--) {
			printf("\nstack[%d]=%ld\n", x, stack[x]);
		}
	}

	buffer[dot] = ch;
	if (dot+1 < BUFFSIZE) buffer[dot+1]=0;
	dot++;
	cputc(ch);	// echo
	return IDLE;
}

void ok() {
	printf(" k\n");
}

char printable[33];
void accept(char token, char *text, int len) {
	long v;
	int n = len > 0 ? len : 0;
	n = len < 33 ? n : 32;
	strncpy(printable, text, n);
	printable[n]=0;
	if (token == OTHER) {
		int n = locate(printable);
		if (n > 0) {
			xt_call(n);
			ok();
		} else {
			printf("?\n");
		}
	} else if (token == NUMBER) {
		if (sp >= STACKSIZE-1) {
			printf("Stack is full!\n");
		} else {
			v = atol(printable);
			stack[sp++] = v;
		}
	} else if (token == OPERATOR) {
		operator = select(printable[0]);
	}
}

int select(char ch) {
	if (ch=='+') return O_ADD;
	if (ch=='-') return O_SUB;
	if (ch=='*') return O_MUL;
	if (ch=='/') return O_DIV;
	if (ch=='%') return O_MOD;
	return O_NONE;
}
void evaluate()
{
	printf("\n");
	if (sp < 1) {
		printf("A suffusion of yellow.\n");
		sp = 0;
	} else if (sp == 1) {
		printf("= %ld\n", stack[0]);
	} else {
		switch (operator) {
			case O_ADD:
				sp--;
				stack[sp-1] += stack[sp];
				operator = O_NONE;
				break;
			case O_SUB:
				sp--;
				stack[sp-1] -= stack[sp];
				operator = O_NONE;
				break;
			case O_MUL:
				sp--;
				stack[sp-1] *= stack[sp];
				operator = O_NONE;
				break;
			case O_DIV:
				sp--;
				stack[sp-1] /= stack[sp];
				operator = O_NONE;
				break;
			case O_MOD:
				sp--;
				stack[sp-1] %= stack[sp];
				operator = O_NONE;
				break;
		}
		printf("= %ld OK\n", stack[sp-1]);
	}

}
