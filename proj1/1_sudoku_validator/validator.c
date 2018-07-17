/*
 * COSC 361 - Project 1A: Multithreaded Sudoku Validator
 * 
 * Description:
 *     Uses threads to check if a sudoku puzzle is valid.
 *     This program was written to support more than 9x9,
 *     but for the sake of the assignment, we will assume
 *     all puzzles are 9x9.
 *
 * Author:
 *     Clara Van Nguyen
 */

//Classic C Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

//CNDS Includes
#include "../lib/handy/cnds/cn_grid.h"

typedef unsigned int       uint;
typedef unsigned long long u64;
typedef unsigned short     ushort;
typedef unsigned char      cn_bool;

typedef struct thread_check {
	ushort           column,
	                 row,
	                 grid;

	cn_bool          comp1,
	                 comp2,
	                 comp3;

	pthread_mutex_t  bool_lock;

	//pthread_cond_t  bool_sig;
} THREAD_CHECK;

typedef struct arg_pair {
	void* arg1,
	    * arg2;
} ARG_PAIR;

cn_bool validate_column(CN_GRID board, uint col) {
	ushort validator = 0,
	       valid;
	uint i = 0;
	char val;
	
	for (; i < cn_grid_size_x(board); i++) {
		val = cn_grid_get(board, char, i, col) - 1;
		valid = validator & (0x1 << val);
		if (valid != 0x0)
			break;
		validator |= (0x1 << val);
	}

	return (validator == 0x1FF);
}

cn_bool validate_row(CN_GRID board, uint row) {
	ushort validator = 0,
	       valid;
	uint i = 0;
	char val;

	for (; i < cn_grid_size_y(board); i++) {
		val = cn_grid_get(board, char, row, i) - 1;
		valid = validator & (0x1 << val);
		if (valid != 0x0)
			break;
		validator |= (0x1 << val);
	}

	return (validator == 0x1FF);
}

cn_bool validate_grid(CN_GRID board, uint _x, uint _y) {
	ushort validator = 0,
		   valid;
	uint a, b = 0;
	char val;

	for (; b < 3; b++) {
		for (a = 0; a < 3; a++) {
			val = cn_grid_get(board, char, (_x * 3) + a, (_y * 3) + b) - 1;
			valid = validator & (0x1 << val);
			if (valid != 0x0)
				break;
			validator |= (0x1 << val);
		}
	}

	return (validator == 0x1FF);
}

cn_bool validate_sudoku(CN_GRID board) {
	//Old-Fashioned, non-threaded version
	uint a;
	cn_bool valid = 1; //Assume true until proven false
	for (a = 0; a < 9; a++) {
		if (validate_column(board, a           ) == 0 ||
		    validate_row   (board, a           ) == 0 ||
		    validate_grid  (board, a % 3, a / 3) == 0) {
			valid = 0;
		}
	}
	return valid;
}

void *__THRD_CHK_COLS(void* arg) {
	//Parse Pointer Data
	ARG_PAIR     *argp = (ARG_PAIR *)     arg;
	CN_GRID       grid = (CN_GRID)        argp->arg1;
	THREAD_CHECK *_CHK = (THREAD_CHECK *) argp->arg2;
	
	//Process the columns
	uint a = 0;
	cn_bool val;
	for (; a < cn_grid_size_y(grid); a++) {
		val = validate_column(grid, a);
		
		if (val == 1)
			_CHK->column |= (0x1 << a);
	}
	
	//Mark as completed
	pthread_mutex_lock(&_CHK->bool_lock);
	_CHK->comp1 = 1;
	pthread_mutex_unlock(&_CHK->bool_lock);

	return NULL;
}

void *__THRD_CHK_ROWS(void* arg) {
	//Parse Pointer Data
	ARG_PAIR     *argp = (ARG_PAIR *)     arg;
	CN_GRID       grid = (CN_GRID)        argp->arg1;
	THREAD_CHECK *_CHK = (THREAD_CHECK *) argp->arg2;

	//Process the rows
	uint a = 0;
	cn_bool val;
	for (; a < cn_grid_size_x(grid); a++) {
		val = validate_row(grid, a);
		
		if (val == 1)
			_CHK->row |= (0x1 << a);
	}
	
	//Mark as completed
	pthread_mutex_lock(&_CHK->bool_lock);
	_CHK->comp2 = 1;
	pthread_mutex_unlock(&_CHK->bool_lock);

	return NULL;
}


void *__THRD_CHK_GRID(void* arg) {
	//Parse Pointer Data
	ARG_PAIR     *argp = (ARG_PAIR *)     arg;
	CN_GRID       grid = (CN_GRID)        argp->arg1;
	THREAD_CHECK *_CHK = (THREAD_CHECK *) argp->arg2;
	
	//Process the grids
	uint a = 0;
	cn_bool val;
	for (; a < cn_grid_size_x(grid); a++) {
		val = validate_grid(grid, a % 3, a / 3);
		
		if (val == 1)
			_CHK->grid |= (0x1 << a);
	}
	
	//Mark as completed
	pthread_mutex_lock(&_CHK->bool_lock);
	_CHK->comp3 = 1;
	pthread_mutex_unlock(&_CHK->bool_lock);

	return NULL;
}

cn_bool validate_sudoku_threaded(CN_GRID board) {
	//Initialise Values
	pthread_t ROW_CHK,
	          COL_CHK,
	          GRI_CHK;

	THREAD_CHECK _CHK;
	_CHK.column = 0;
	_CHK.row    = 0;
	_CHK.grid   = 0;

	pthread_mutex_init(&_CHK.bool_lock, NULL);

	ARG_PAIR argp;
	argp.arg1 = (void *) board; //CN_GRID = *cn_grid #DrPlankFTW
	argp.arg2 = (void *) &_CHK;
		
	if (pthread_create(&COL_CHK, NULL, &__THRD_CHK_COLS, (void *) &argp) != 0) {
		fprintf(stderr, "[ \e[1;31mERROR\e[0m ] Column Thread Failed to Run (Error Code: 11)\n");
		cn_grid_free(board);
		exit(11);
	}
	
	if (pthread_create(&ROW_CHK, NULL, &__THRD_CHK_ROWS, (void *) &argp) != 0) {
		fprintf(stderr, "[ \e[1;31mERROR\e[0m ] Row Thread Failed to Run (Error Code: 12)\n");
		cn_grid_free(board);
		exit(12);
	}

	if (pthread_create(&GRI_CHK, NULL, &__THRD_CHK_GRID, (void *) &argp) != 0) {
		fprintf(stderr, "[ \e[1;31mERROR\e[0m ] Grid Thread Failed to Run (Error Code: 13)\n");
		cn_grid_free(board);
		exit(13);
	}
	
	//Wait until completion.
	while (1) {
		pthread_mutex_lock(&_CHK.bool_lock);
		if (_CHK.comp1 == 1 && _CHK.comp2 == 1 && _CHK.comp3 == 1) {
			pthread_mutex_unlock(&_CHK.bool_lock);
			break;
		}
		pthread_mutex_unlock(&_CHK.bool_lock);
	}
	
	pthread_join(ROW_CHK, NULL);
	pthread_join(COL_CHK, NULL);
	pthread_join(GRI_CHK, NULL);
	
	//Guaranteed to run after threads are done calculating
	//Return the result.
	return (_CHK.row == 0x1FF && _CHK.column == 0x1FF && _CHK.grid == 0x1FF);
}

main(int argc, char** argv) {
	if (argc != 2) {
		//Clearly you have no idea what you are doing...
		fprintf(stderr, "Usage: validator file\n");
		exit(0);
	}

	uint w, h;
	int v, r;
	FILE* fp = fopen(argv[1], "r");
	fscanf(fp, "%d %d", &w, &h);

	//Create the game board
	CN_GRID board = cn_grid_init_size(char, w, h);

	//Read values into CN_Grid
	for (h = 0; h < cn_grid_size_y(board); h++) {
		for (w = 0; w < cn_grid_size_x(board); w++) {
			//Error check whether or not the file is empty.
			v = -1;
			fscanf(fp, "%d", &v);
			if (v == -1) {
				fprintf(stderr, "[ \e[1;31mERROR\e[0m ] File not large enough. (Error Code: 14)\n");
				fclose(fp);
				cn_grid_free(board);
				exit(14);
			}
			
			cn_grid_get(board, char, w, h) = (char)v;
		}
	}
	fclose(fp);
	
	//Non-Threaded Solution
	/*uint a = 0;
	printf("%d\n", (int)validate_sudoku(board));*/

	//Now for the fun part.
	//Threaded Solution
	cn_bool valid = validate_sudoku_threaded(board); 
	printf("%d\n", (int)valid);

	//Free Memory
	cn_grid_free(board);
	
	//Have a nice day
	return valid;
}
