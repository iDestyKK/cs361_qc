# COSC 361 - Programming Project 3 Makefile

CC = gcc
CFLAGS = --std=gnu89 -g
RI = lib/handy/cnds/cn_vec.c lib/handy/file.c lib/handy/stream.c

all: disksim

disksim:
	$(CC) $(CFLAGS) -o disksim disksim.c $(RI)

clean:
	rm disksim
