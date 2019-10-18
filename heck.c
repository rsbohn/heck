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

void ok();

char buffer[BUFFSIZE];
int dot = 0;
int prevtoken = IDLE;
int operator = O_NONE;

#define PROMPT "heck: "

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
			operator = evaluate(operator);
			ok();
			printf("= %ld\n", top());
			printf(PROMPT);
		}
	} while (last != GEOF);
	return 0;
}

void ok() {
	cbm_k_bsout(0xBB);
	cbm_k_bsout(0x0D);
}

#define CTRLD 0x04
#define CTRLH 0x14 /* ??? */
int reader() 
{
	char ch;
	if (dot >= BUFFSIZE) return BFULL;
	if (!kbhit()) return IDLE;
	ch=cgetc();
	if (ch==CTRLD) {
		return GEOF;
	}
	if (ch==CTRLH) {
		dot-=1;
		if (dot < 0) dot = 0;
		else {
			cbm_k_bsout(CH_CURS_LEFT);
			cbm_k_bsout(' ');
			cbm_k_bsout(CH_CURS_LEFT);
		}
		buffer[dot]=0;
		return IDLE;
	}
	if (ch=='=' || ch==0x0D) {
		scan(buffer);
		dot = 0;
		buffer[dot]=0;
		return RESOLVE;
	}

	buffer[dot] = ch;
	if (dot+1 < BUFFSIZE) buffer[dot+1]=0;
	dot++;
	cputc(ch);	// echo
	return IDLE;
}

char printable[33];
parser_state_t accept(char token, char *text, int len) {
	int n = len > 0 ? len : 0;
	n = len < 33 ? n : 32;
	strncpy(printable, text, n);
	printable[n]=0;
	if (token == OTHER) {
		int n = locate(printable);
		if (n > 0) {
			xt_call(n);
		} else {
			printf("?\n");
			return PARSE_BOGUS;
		}
	} else if (token == NUMBER) {
		push(atol(printable));
	} else if (token == OPERATOR) {
		operator = select(printable[0]);
	}
	return PARSE_OK;
}

int select(char ch) {
	if (ch=='+') return O_ADD;
	if (ch=='-') return O_SUB;
	if (ch=='*') return O_MUL;
	if (ch=='/') return O_DIV;
	if (ch=='%') return O_MOD;
	return O_NONE;
}
