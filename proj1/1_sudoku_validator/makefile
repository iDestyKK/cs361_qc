CC = gcc
CFLAGS = --std=gnu89 -g -lpthread

$validator:
	$(CC) $(CFLAGS) -o validator validator.c ../lib/handy/cnds/cn_grid.c

clean:
	rm validator
