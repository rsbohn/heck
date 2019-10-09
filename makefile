CC=cl65 -t cx16

HECK.PRG: heck.c
	$(CC) -o $@ $<

TD.PRG: tiledemo.c
	$(CC) -o $@ $<
clean:
	-@rm dump*
