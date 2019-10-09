#include <stdio.h>
#include <stdint.h>
#include <conio.h>

#define BANKF ((uint8_t) 0x0F)
#define LAYER1 ((uint8_t) 0x3000)
uint8_t *vera = (uint8_t *) 0x9F20;
volatile uint8_t *vr0 = (uint8_t *) 0x9F23;

void vpoke(uint8_t bank, uint16_t addr, uint8_t value);
void vmode3();
void mktiles();
void tput(uint8_t x, uint8_t y, uint8_t n);
void pfill();
void pset(uint8_t slot, uint8_t r, uint8_t g, uint8_t b);

#define ROW0 4
void main() 
{
	uint8_t c,x,y,n;
	uint8_t *m = (uint8_t *)646;
	m[0] = 1;
	pfill();
	vmode3();
	mktiles();
	y = ROW0;
	n = 1;
	c = 0;
	do {
		waitvsync();
		for (x = 0; x < 20; x+=2) {
			tput(x,y,n);
			tput(x+1,y,n+1);
		}
		y++;
		if (y > ROW0+4) { c++; y = ROW0; }
		n++;
		if (n>3) { n = 1; }
	} while (!kbhit());
}

void vpoke(uint8_t bank, uint16_t addr, uint8_t value) 
{
	vera[0] = addr & 0xFF;
	vera[1] = addr >> 8;
	vera[2] = bank;
	vera[4] = 0x00;
	*vr0 = value;
}

void vmode3() 
{
	vpoke(15,1,64);
	vpoke(15,2,64);
	vera[0] = 0x00;
	vera[1] = 0x30;
	vera[2] = 0x1F;
	vera[4] = 0x00;

	*vr0 = (uint8_t) 0x61;
	*vr0 = (uint8_t) 0x35;
	*vr0 = (uint8_t) 0x00;
	*vr0 = (uint8_t) 0x10;
	*vr0 = (uint8_t) 0x00;
	*vr0 = (uint8_t) 0x12;
}

void mktiles() 
{
	uint16_t addr;
	for (addr = 0; addr < 128; addr++) {
		vpoke(0, 0x4880+addr, 0x4F);
		vpoke(0, 0x4900+addr, 0x5D);
		vpoke(0, 0x4980+addr, 0x6E);
		vpoke(0, 0x4A00+addr, 0x7C);
	}
}

void tput(uint8_t x, uint8_t y, uint8_t n) 
{
	vpoke(0, 0x4000+128*y+2*x, n);
}


void pfill()
{
	/* velocity-blue */
	pset(0,1,2,4);
	pset(1,1,2,5);
	pset(2,2,3,7);
	pset(3,1,4,8);
	pset(4,1,5,9);
	pset(5,1,6,10);
	pset(6,2,7,10);
	pset(7,2,8,10);
	pset(8,3,9,10);
	pset(9,4,10,10);
	pset(10,6,11,11);
	pset(11,8,12,11);
	pset(12,10,12,12);
	pset(13,12,13,13);
	pset(14,14,14,13);
	pset(15,15,15,14);
}

void pset(uint8_t slot, uint8_t r, uint8_t g, uint8_t b)
{
	vpoke(15, 0x1000+slot*2+1, r);
	vpoke(15, 0x1000+slot*2, g*16+b);
}

