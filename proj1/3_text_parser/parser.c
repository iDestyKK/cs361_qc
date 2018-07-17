/*
 * COSC 361 - Project 1C: Multithreaded Text Parser
 * 
 * Description:
 *     This program takes a text file, reads every line, and then, with
 *     threads, will parse each line and change certain attributes of it. In
 *     this program, we will replace every space with an asterisk (*) and
 *     every lower case letter with a capital letter. It will have four
 *     threads in the following process:
 *     
 *     Reader -> Processing Step 1 -> Processing Step 2 -> Writer
 *
 *     Thread 1 will read all lines. Then it will pass each line to the next
 *     thread to change every space to asterisks. Then pass that to thread
 *     three for capitalisation. Finally, when all threads are completed, it
 *     will print out the entire file.
 *
 * Compilation:
 *     This program was written under the C GNU89 standard. As such, it should
 *     be compiled under the following command:
 *     
 *     gcc --std=gnu89 -lpthread -o main main.c handy/file.c handy/strmanip.c
 * 
 * Author:
 *     Clara Van Nguyen
 */

//C Includes
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//Handy Includes
#include "../lib/handy/types.h"          //cn_uint
#include "../lib/handy/file.h"           //file_exists(...), file_to_array(...)
#include "../lib/handy/cnds/cn_queue.h"  //CN_QUEUE

typedef struct var_mgmt {
	cn_uint         line_num;    //Number of lines

	char*           fname,       //Filename to read from
	    *           oname;       //Filename to write to
	char**          lines;       //Lines in file
	
	//NOTE: CN_QUEUE is a defined as "cn_queue*"
	CN_QUEUE        init_queue,  //Queue to store original string
	                space_queue, //Queue to handle space conversion
	                case_queue;  //Queue to handle upper-casing

	pthread_mutex_t queue_lock1, //Init Queue Lock
	                queue_lock2, //Space Queue Lock
	                queue_lock3; //Case Queue Lock
} VAR_MGMT;

void parse_spaces(char* str) {
	cn_uint i = 0;
	for (; str[i] != 0x0; i++) {
		if (str[i] == ' ')
			str[i] = '*';
	}
}

void parse_lowercase(char* str) {
	cn_uint i = 0;
	for (; str[i] != 0x0; i++) {
		if (str[i] >= 'a' && str[i] <= 'z')
			str[i] += 'A' - 'a'; //Yes
	}
}

//Thread Functions
void *__THRD_READ(void* arg) {
	//This thread reads from the file and puts each line in its initial queue.
	VAR_MGMT* mgr = (VAR_MGMT *) arg;
	
	//Read file into NULL-terminated char** array.
	mgr->lines = file_to_array(mgr->fname);

	//Copy each line into the cn_queue
	cn_uint i = 0;
	pthread_mutex_lock(&mgr->queue_lock1);
	for (; mgr->lines[i] != NULL; i++) {
		cn_queue_push(mgr->init_queue, &mgr->lines[i]);
	}
	mgr->line_num = i;
	pthread_mutex_unlock(&mgr->queue_lock1);

	return NULL;
}

void *__THRD_SPACE(void* arg) {
	//This thread replaces all spaces with asterisks
	//In doing so, it will take entries from init_queue and copy
	//them to space_queue.

	VAR_MGMT* mgr = (VAR_MGMT *) arg;
	cn_uint ln = 0;
	
	while (ln < mgr->line_num) {
		//Read from the first queue and process entries
		pthread_mutex_lock(&mgr->queue_lock1);
		
		while (!cn_queue_empty(mgr->init_queue)) {
			char* str_ptr = *(char**) cn_queue_front(mgr->init_queue);
			parse_spaces(str_ptr);

			pthread_mutex_lock(&mgr->queue_lock2);
			cn_queue_push(mgr->space_queue, &str_ptr);
			pthread_mutex_unlock(&mgr->queue_lock2);

			cn_queue_pop(mgr->init_queue);
			ln++;
		}

		pthread_mutex_unlock(&mgr->queue_lock1);
	}

	return NULL;
}

void *__THRD_CASE(void* arg) {
	//This thread replaces all lower case characters with upper cases
	//In doing so, it will take entries from space_queue and copy
	//them to case_queue.

	VAR_MGMT* mgr = (VAR_MGMT *) arg;
	cn_uint ln = 0;
	
	while (ln < mgr->line_num) {
		//Read from the first queue and process entries
		pthread_mutex_lock(&mgr->queue_lock2);
		
		while (!cn_queue_empty(mgr->space_queue)) {
			char* str_ptr = *(char**) cn_queue_front(mgr->space_queue);
			parse_lowercase(str_ptr);

			pthread_mutex_lock(&mgr->queue_lock3);
			cn_queue_push(mgr->case_queue, &str_ptr);
			pthread_mutex_unlock(&mgr->queue_lock3);

			cn_queue_pop(mgr->space_queue);
			ln++;
		}

		pthread_mutex_unlock(&mgr->queue_lock2);
	}

	return NULL;
}

void *__THRD_FINAL(void* arg) {
	//This thread simply takes all of the strings in case_queue and
	//prints them to wherever the user defined their output to be.

	VAR_MGMT* mgr = (VAR_MGMT *) arg;
	cn_uint ln = 0;
	FILE* write_to = NULL;

	if (mgr->oname != NULL)
		write_to = fopen(mgr->oname, "w");

	while (ln < mgr->line_num) {
		//Read from the queue and print out
		pthread_mutex_lock(&mgr->queue_lock3);
		while (!cn_queue_empty(mgr->case_queue)) {
			char* str_ptr = *(char**) cn_queue_front(mgr->case_queue);
			if (write_to != NULL)
				fprintf(write_to, "%s\n", str_ptr);
			else
				printf("%s\n", str_ptr);

			cn_queue_pop(mgr->case_queue);
			ln++;
		}
		pthread_mutex_unlock(&mgr->queue_lock3);
	}

	if (write_to != NULL)
		fclose(write_to);

	return NULL;
}

//Main (Initial) Thread
main(int argc, char** argv, char** envp) {
	//Error check, before we do anything
	if (argc == 1) {
		fprintf(stderr, "Usage: parser file [output_file]\n\nReplaces all lower case letters with capital letters.\nAlso replaces all spaces with asterisks.\n\nIf \"output_file\" is not specified, the result will be printed to stdout.\n");
		exit(2);
	}
	
	if (argc > 3) {
		fprintf(stderr, "[ \e[1;31;mERROR\e[0m ] Expected 1 argument, got %d\n", argc - 1);
		exit(3);
	}

	//Check if file exists
	if (file_exists(argv[1]) == cn_false) {
		fprintf(stderr, "[ \e[1;31;mERROR\e[0m ] File \"%s\" does not exist.\n", argv[1]);
		exit(4);
	}

	//Create variable manager
	VAR_MGMT mgr;

	//Create the queues
	mgr.init_queue  = cn_queue_init(char**);
	mgr.space_queue = cn_queue_init(char**);
	mgr.case_queue  = cn_queue_init(char**);

	//Initialise Mutexes
	pthread_mutex_init(&mgr.queue_lock1, NULL);
	pthread_mutex_init(&mgr.queue_lock2, NULL);
	pthread_mutex_init(&mgr.queue_lock3, NULL);

	//Initialise Threads
	pthread_t _PTHD_READ,
	          _PTHD_SPACE,
	          _PTHD_CASE,
	          _PTHD_FINAL;

	mgr.fname = argv[1];

	if (argc == 3)
		mgr.oname = argv[2];
	
	//Begin threads, executed backwards
	if (pthread_create(&_PTHD_FINAL, NULL, &__THRD_FINAL, (void *) &mgr)) {
		fprintf(stderr, "[ \e[1;31;mERROR\e[0m ] Failed to execute WRITE thread (Error Code: 15)\n");
		exit(15);
	}

	if (pthread_create(&_PTHD_CASE, NULL, &__THRD_CASE, (void *) &mgr)) {
		fprintf(stderr, "[ \e[1;31;mERROR\e[0m ] Failed to execute CASE_UPPER thread (Error Code: 14)\n");
		exit(14);
	}

	if (pthread_create(&_PTHD_SPACE, NULL, &__THRD_SPACE, (void *) &mgr)) {
		fprintf(stderr, "[ \e[1;31;mERROR\e[0m ] Failed to execute SPACE thread (Error Code: 13)\n");
		exit(13);
	}
	
	if (pthread_create(&_PTHD_READ, NULL, &__THRD_READ, (void *) &mgr)) {
		fprintf(stderr, "[ \e[1;31;mERROR\e[0m ] Failed to execute READ thread (Error Code: 12)\n");
		exit(12);
	}

	pthread_join(_PTHD_READ , NULL); //Wait for everything to be read
	pthread_join(_PTHD_SPACE, NULL); //Wait for everything to be spaced
	pthread_join(_PTHD_CASE , NULL); //Wait for everything to be upper cased
	pthread_join(_PTHD_FINAL, NULL); //Wait for everything to be printed out

	//Free Memory
	cn_queue_free(mgr.init_queue);
	cn_queue_free(mgr.space_queue);
	cn_queue_free(mgr.case_queue);
	
	cn_uint i = 0;
	for (; mgr.lines[i] != NULL; i++) {
		free(mgr.lines[i]);
	}
	free(mgr.lines);
}
