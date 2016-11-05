# COSC 361 - Programming Project 2 Makefile

CC = gcc
CFLAGS = --std=gnu89 -g
RI = lib/handy/cnds/cn_vec.c lib/handy/file.c lib/handy/stream.c lib/handy/cnds/cn_list.c img_helper.c lib/PPM_Edit/ppm_edit.c

all: fcfs rr sjf priority

# Perform Awesomeness...
fcfs:
	$(CC) $(CFLAGS) -o fcfs fcfs.c $(RI)

rr:
	$(CC) $(CFLAGS) -o rr rr.c $(RI)

sjf:
	$(CC) $(CFLAGS) -o sjf sjf.c $(RI)

priority:
	$(CC) $(CFLACS) -o priority priority.c $(RI)

clean:
	rm fcfs rr sjf
