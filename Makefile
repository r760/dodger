# makefile for dodger
#
# make dodger

CC=gcc
CFLAGS=-lncurses

clean:
	rm -rf dodger

dodger: dodger.c
	$(CC) $(CFLAGS) dodger.c -o dodger
