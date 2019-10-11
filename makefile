ALL: unit TEST.PRG HECK.PRG

CC=cl65 -t cx16
XHOME=~/x16emu

unit: test.c scan.h scan.c
	cc -o $@ test.c scan.c

HECK.PRG: heck.c scan.c
	$(CC) -o $@ heck.c scan.c

TD.PRG: tiledemo.c
	$(CC) -o $@ $<

TEST.PRG: test.c scan.c scan.h
	$(CC) -o $@ test.c scan.c




clean:
	-@rm dump* memory.bin
