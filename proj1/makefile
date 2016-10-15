# COSC 361 - Programming Project 1 Makefile

CC = gcc
CFLAGS = --std=gnu89 -g -lpthread

all: validator parser

# Perform Awesomeness...
validator:
	$(CC) $(CFLAGS) -o validator 1_sudoku_validator/validator.c lib/handy/cnds/cn_grid.c

parser:
	$(CC) $(CFLAGS) -o parser 3_text_parser/parser.c lib/handy/cnds/cn_queue.c lib/handy/file.c

clean:
	rm validator parser
