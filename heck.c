#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>

#include "heck.h"

char buffer[BUFFSIZE];
int dot = 0;
int prevtoken = IDLE;
int oper = O_NONE;
#define STACKSIZE 8
int stack[STACKSIZE];
int sp = STACKSIZE-1;

char optable[] = "?+-*/%";

void main()
{
	int last;
	do {
		last = readToken();
		if (last == RESOLVE) evaluate();
	} while (last != GEOF);
}

#define CTRLD 0x04
#define CTRLH 0x08
int readToken() 
{
	char ch;
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
		accept(prevtoken);
		prevtoken = IDLE;
		dot = 0;
		return RESOLVE;
	}
	if (ch==0x20 || ch==0x0D) {
		accept(prevtoken);
		prevtoken=IDLE;
		dot = 0;
		return IDLE;
	}
	if (isdigit(ch)) {
		if (prevtoken != NUMBER) {
			accept(prevtoken);
			prevtoken = NUMBER;
			dot = 0;
		}
	} else {
		if (prevtoken == NUMBER) {
			accept(prevtoken);
			if (O_NONE==select(ch)) {
				prevtoken = OTHER;
			} else {
				prevtoken = OPERATOR;
			}
			dot = 0;
		}
	}
	buffer[dot] = ch;
	if (dot+1 < BUFFSIZE) buffer[dot+1]=0;
	dot++;
	cputc(ch);	// echo
	return IDLE;
}

void accept(int token) {
	if (token == NUMBER) {
		stack[sp--] = atoi(buffer);
	} else if (token == OPERATOR) {
		oper = select(buffer[0]);
	}
	printf("\n%d:%s\n", token, buffer);
}

int select(char ch) {
	printf("\nselect:%c\n",ch);
	if (ch=='+') return O_ADD;
	if (ch=='-') return O_SUB;
	if (ch=='*') return O_MUL;
	if (ch=='/') return O_DIV;
	if (ch=='%') return O_MOD;
	return O_NONE;
}
void evaluate()
{
	printf("\nop:%c\n", optable[oper]);
	printf("A suffusion of yellow.\n");
}

void print() {
	printf("\n%s\n:", buffer);
}
