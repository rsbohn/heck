// dict.c
// Copyright (c) Randall Bohn 2019
// BSD 2 Clause License

#include <conio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "dict.h"

#define STACKSIZE 8
#define SMASK 7
long stack[STACKSIZE];
int sp = 0;
#define TOS stack[sp]

void push(long v)
{
	sp = (sp+1) & SMASK;
	stack[sp] = v;
}

long pop()
{
	int a = TOS;
	sp = (sp - 1) & SMASK;
	return a;
}

long top()
{
	return TOS;
}

#define NAMED(x,y) const char x[]=y
NAMED(_nop, " ");
void nop() {}

NAMED(_words, "words");
void words();

NAMED(_dup, "dup");
void dup()
{
	push(TOS);
}

NAMED(_drop, "drop");
void drop()
{
	pop();
}

NAMED(_over, "over");
void over()
{
	long v = stack[(sp-1)&SMASK];
	push(v);
}
NAMED(_dot, ".");
void fdot()
{
	char s[9];
	long v = pop();
	ltoa(v, s, 10);
	cputs(s);
	cputc(' ');
}
NAMED(_dots, "stack");
void fdots()
{
	int i;
	cputc(' ');
	for (i = 0; i < STACKSIZE; i++) {
		fdot();
	}
}

NAMED(_fetch, "@");
void fetch()
{
	uint8_t *addr = (uint8_t*)(pop() & 0xFFFF);
	uint8_t u = *addr;
	push(0L + u);
}
NAMED(_store, "!");
void store()
{
	uint16_t addr = (uint16_t)(pop() & 0xFFFF);
	uint8_t u = (uint8_t)(pop() & 0xFF);
	*(uint8_t *)addr = u;
}
NAMED(_hook, "?");
void hook()
{
	cputc(' ');
	fetch();
	fdot();
}
NAMED(_emit, "emit");
void emit()
{
	cputc((char)pop());
}

/***************/
typedef struct
{
	const char *name;
	void (*function)();
} entry;

const entry dictionary[] = 
{
	{_nop, nop}
,	{_words, words}
,	{_dup, dup}
,	{_drop, drop}
,	{_over, over}
,	{_dot, fdot}
,	{_dots, fdots}
,	{_fetch, fetch}
,	{_store, store}
,	{_hook, hook}
,	{_emit, emit}
};
int entries = sizeof dictionary / sizeof dictionary[0];

void words() 
{
	int i;
	gotox(0);
	cputs("\n");
	for (i = entries; i > 0; i--) {
		cputs(dictionary[i].name);
		cputs(" ");
	}
}

int locate(char *word)
{
	int i;
	for (i = entries; i >= 0; i--) {
		if (!strcmp(word, dictionary[i].name)) return i;
	}
	return 0;
}

int xt_call(int entry)
{
	if (entry > 0) dictionary[entry].function();
	return entry;
}

/* do 4+1 function math */
int evaluate(int operator)
{
	int a;
	if (O_NONE == operator) return operator;
	a = pop();
	switch (operator) {
		case O_ADD:
			TOS += a;
			break;
		case O_SUB:
			TOS -= a;
			break;
		case O_MUL:
			TOS *= a;
			break;
		case O_DIV:
			TOS /= a;
			break;
		case O_MOD:
			TOS %= a;
			break;
	}
	return O_NONE;
}
