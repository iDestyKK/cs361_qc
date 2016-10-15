CC = gcc
CFLAGS = --std=gnu89 -g -lpthread

$parser:
	$(CC) $(CFLAGS) -o parser parser.c ../lib/handy/file.c ../lib/handy/cnds/cn_queue.c

clean:
	rm parser
