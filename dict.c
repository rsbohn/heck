// dict.c
// Copyright (c) Randall Bohn 2019
// BSD 2 Clause License

#include <conio.h>
#include <string.h>

#define NAMED(x,y) const char x[]=y
NAMED(_nop, " ");
void nop() {}

NAMED(_words, "words");
void words();

NAMED(_dup, "dup");
void dup(); /* defined in heck.c */

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
