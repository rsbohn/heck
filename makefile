CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -pedantic -Wno-unused-parameter

.PHONY: all clean

all: hal9000

hal9000: heck.c heck.h
	$(CC) $(CFLAGS) -o $@ heck.c

unit: test.c scan.c scan.h
	$(CC) $(CFLAGS) -o $@ test.c scan.c

clean:
	rm -f hal9000 unit
