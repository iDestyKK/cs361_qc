# COSC 361 - Programming Project 2 Makefile

CC = gcc
CFLAGS = --std=gnu89 -g

all: fcfs rr

# Perform Awesomeness...
fcfs:
	$(CC) $(CFLAGS) -o fcfs fcfs.c lib/cn_vec/cn_vec.c lib/handy/file.c lib/handy/stream.c

rr:
	$(CC) $(CFLAGS) -o rr rr.c lib/cn_vec/cn_vec.c lib/handy/file.c lib/handy/stream.c lib/handy/cnds/cn_queue.c lib/handy/cnds/cn_list.c

clean:
	rm fcfs rr
