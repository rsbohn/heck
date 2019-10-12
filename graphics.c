// graphics.c
// Copyright (c) 2019 Randall Bohn
// BSD 2 Clause License

#include <stdint.h>
#include <stdio.h>
#include <cbm.h>
#define WHITE_ON_BLACK 1

uint8_t *screen_color = (uint8_t *) 646;
uint8_t *screen_width = (uint8_t *) 217;
uint8_t *screen_height = (uint8_t *) 218;

void vpoke (uint8_t bank, uint16_t address, uint8_t data)
{
	VERA.control = 0;
	VERA.address_hi = bank;
	VERA.address = address;
	VERA.data0 = data;
}

#define HSCALE 2
void big_pixels()
{
	vpoke(15, HSCALE, 64);
	*screen_height = 30;
}
void he_logo()
{
	*screen_color = WHITE_ON_BLACK;
	printf("%c", 147);
	printf("The Heck Calculator *PROTOTYPE*\n\n");
	printf("  Enter an expression.\n");
	printf("  * / %%, + - are available.\n");
	printf("  16 bit integers only!\n");
	printf("  Press . to view the stack.\n\n");
}