CC=cl65 -t cx16
XHOME=~/x16emu

HECK.PRG: heck.c
	$(CC) -o $@ $<

TD.PRG: tiledemo.c
	$(CC) -o $@ $<
clean:
	-@rm dump*
