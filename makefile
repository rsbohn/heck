ALL: unit TEST.PRG HECK.PRG

CC=cl65 -t cx16

unit: test.c scan.h scan.c
	cc -o $@ test.c scan.c

OBJLIST=scan.o graphics.o
HECK.PRG: heck.c $(OBJLIST)
	$(CC) -o $@ $< $(OBJLIST)

TD.PRG: tiledemo.c
	$(CC) -o $@ $<

TEST.PRG: test.c scan.c scan.h
	$(CC) -o $@ test.c scan.c




clean:
	-@rm dump* memory.bin
