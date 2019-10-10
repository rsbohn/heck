ALL: trial HECK.PRG

CC=cl65 -t cx16
XHOME=~/x16emu

trial: test.c scan.h scan.c
	cc -o $@ test.c scan.c

HECK.PRG: heck.c
	$(CC) -o $@ $<

TD.PRG: tiledemo.c
	$(CC) -o $@ $<




clean:
	-@rm dump* memory.bin
