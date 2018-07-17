/*
 * COSC 361 - Project 1B: Dining Philosophers
 * 
 * Description:
 *     Dining Philosophers is a common problem in computer science. The problem
 *     consists of philosophers and chopsticks (or forks, if you prefer). There
 *     are the same number of chopsticks as there are philosophers. The goal is
 *     to prevent deadlocks.
 * 
 *     NOTE: This is a modified version of "philosophers_orig.c" suited for the
 *           textbook assignment for the class. As a result, this one is the
 *           target for makefiles and should be the one compiled.
 * 
 * Author:
 *     Clara Van Nguyen
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#include "../lib/handy/handy.h" 
#include "../lib/handy/types.h" //cn_uint, cn_bool

#define PH_NUM 5

typedef enum {
	STATE_THINKING,
	STATE_EATING
} STATE;

//Pair Struct
typedef struct pair {
	int   left;
	void* right;
} PAIR;

//Chopstick Struct
typedef struct chopstick {
	cn_bool         picked;
	pthread_mutex_t lock;
	cn_uint         id;
	int             phil_id;
} CHOPSTICK;

//Philosopher Struct
typedef struct philosopher {
	STATE             state;
	char              name;
	struct chopstick* left_chop,
	                * right_chop;
} PHILOSOPHER;

//Struct to manage the program.
typedef struct mgr {
	PHILOSOPHER    * phil;        //Philosopher Structs
	CHOPSTICK      * chop;        //Chopstick Structs
	pthread_t      * phil_thread; //Threads for all philosophers
	pthread_mutex_t  lock,        //Mutex Lock for access
	                 plock;       //Mutex Lock for printf
	pthread_cond_t * cond;        //Condition Variables
	cn_uint          num_up,      //Number of Chopsticks picked up
	                 phil_num;    //Number of Philosophers
	cn_bool          start;       //Bool to control when the threads start
} MGR;

void grabChopstick(MGR* mgmt, cn_uint id, CHOPSTICK* c_id) {
	//Grab the chopstick. The if-statement will go through
	//the moment the mutex is unlocked (if used by another
	//thread)
	if (pthread_mutex_lock(&c_id->lock) == 0) {
		if (c_id->phil_id != -1) {
			pthread_cond_wait(&mgmt->cond[c_id->id], &c_id->lock);
		}
		c_id->phil_id = id;
		c_id->picked = CN_TRUE;
	}
}

void releaseChopstick(MGR* mgmt, cn_uint id, CHOPSTICK* c_id) {
	//Only release mutex if it is actually locked by this
	//thread.
	if (c_id->picked == CN_TRUE && c_id->phil_id == id) {
		c_id->phil_id = -1;
		c_id->picked = CN_FALSE;
		pthread_mutex_unlock(&c_id->lock);
	}
	pthread_cond_signal(&mgmt->cond[c_id->id]);
}

void eat(MGR* mgmt, cn_uint id) {
	pthread_mutex_lock(&mgmt->lock);
	
	pthread_mutex_lock(&mgmt->plock);
	printf("Philosopher %d wants to eat.\n", id);
	fflush(stdout);
	pthread_mutex_unlock(&mgmt->plock);

	double duration = 1000000 + (rand() % 2000000);

	if (mgmt->num_up > mgmt->phil_num - 1) {
		//We can't let all of the chopsticks be picked up at once.
		pthread_mutex_lock(&mgmt->plock);
		printf("Philosopher %d can not eat, as %d chopsticks are picked up.\n", id, mgmt->phil_num - 1);
		fflush(stdout);
		pthread_mutex_unlock(&mgmt->plock);
	}
	else {
		if (id % 2 == 0) {
			//Grab Right first, then left.
			grabChopstick(mgmt, id, mgmt->phil[id].right_chop);
			grabChopstick(mgmt, id, mgmt->phil[id].left_chop);
		}
		else {
			//Vice Versa
			grabChopstick(mgmt, id, mgmt->phil[id].left_chop);
			grabChopstick(mgmt, id, mgmt->phil[id].right_chop);
		}
		pthread_mutex_lock(&mgmt->plock);
		printf("Philosopher %d is eating for %lg seconds.\n", id, duration / 1000000);
		fflush(stdout);
		pthread_mutex_unlock(&mgmt->plock);
		mgmt->phil[id].state = STATE_EATING;
		mgmt->num_up++;
	}
	pthread_mutex_unlock(&mgmt->lock);
	usleep(duration);
}

void think(MGR* mgmt, cn_uint id) {
	pthread_mutex_lock(&mgmt->plock);
	double duration = 1000000 + (rand() % 2000000);
	printf("Philosopher %d is thinking for %lg seconds.\n", id, duration / 1000000);
	fflush(stdout);
	pthread_mutex_unlock(&mgmt->plock);

	releaseChopstick(mgmt, id, mgmt->phil[id].left_chop);
	releaseChopstick(mgmt, id, mgmt->phil[id].right_chop);
	
	if (mgmt->phil[id].state == STATE_EATING) {
		pthread_mutex_lock(&mgmt->lock);
		mgmt->num_up--;
		pthread_mutex_unlock(&mgmt->lock);
		mgmt->phil[id].state = STATE_THINKING;
	}
	usleep(duration);
}

void *__PHILOSOPHER(void *arg) {
	//Philosopher Thread
	//Dereference Pointer Data
	PAIR   *  p    = (PAIR*)   arg;
	MGR    *  mgmt = (MGR *)   p->right;
	cn_uint   id   = (cn_uint) p->left;
	
	/*pthread_mutex_lock(&mgmt->plock);
	printf("Philosopher %d has joined.\n", id);
	fflush(stdout);
	pthread_mutex_unlock(&mgmt->plock);*/
	
	//Loop until ready to start.
	while (mgmt->start == CN_FALSE) {}

	//Work
	while (1) {
		if (id % 2 == 0) {
			eat  (mgmt, id);
			think(mgmt, id);
		}
		else {
			think(mgmt, id);
			eat  (mgmt, id);
		}
	}

	return NULL;
}

main(int argc, char** argv) {
	//Layout the "dinner table".
	MGR mgmt;
	mgmt.phil_num    = PH_NUM;
	mgmt.phil        = (PHILOSOPHER    *) malloc(sizeof(struct philosopher   ) * mgmt.phil_num);
	mgmt.chop        = (CHOPSTICK      *) malloc(sizeof(struct chopstick     ) * mgmt.phil_num);
	mgmt.phil_thread = (pthread_t      *) malloc(sizeof(       pthread_t     ) * mgmt.phil_num);
	mgmt.cond        = (pthread_cond_t *) malloc(sizeof(       pthread_cond_t) * mgmt.phil_num);
	mgmt.num_up      = 0;
	mgmt.start       = CN_FALSE;
	pthread_mutex_init(&mgmt.lock , NULL);
	pthread_mutex_init(&mgmt.plock, NULL);

	//Set up the Random Number Generator
	srand(time(NULL) * getpid());

	cn_uint i = 0;
	PAIR* pairarg = (PAIR *) malloc(sizeof(struct pair) * mgmt.phil_num);
	for (; i < mgmt.phil_num; i++) {
		//Set up Philosopher
		mgmt.phil[i].left_chop  = &mgmt.chop[ i              ];
		mgmt.phil[i].right_chop = &mgmt.chop[(i + 1) % mgmt.phil_num];
		mgmt.phil[i].name       = 'A' + i;
		mgmt.phil[i].state      = STATE_THINKING;
		
		//Set up Chopstick
		mgmt.chop[i].picked  = CN_FALSE;
		mgmt.chop[i].phil_id = -1;
		mgmt.chop[i].id      = i;
		pthread_mutex_init(&mgmt.chop[i].lock, NULL);

		//Set up Condition Variable
		pthread_cond_init(&mgmt.cond[i], NULL);

		//Set up Pair arguments
		pairarg[i].left  = i;
		pairarg[i].right = (void *) &mgmt;
	}
	
	//Run each thread
	for (i = 0; i < mgmt.phil_num; i++) {
		if (pthread_create(&mgmt.phil_thread[i], NULL, __PHILOSOPHER, (void*) (pairarg + i)) != 0) {
			fprintf(stderr, "[ \e[1;31mERROR\e[0m ] Failed to initialise Philosopher thread #%d (Error Code: %d)\n", i + 1, 11 + i);	
		}
	}

	//Start the threads
	mgmt.start = CN_TRUE;

	//Wait for each thread to terminate
	for (i = 0; i < mgmt.phil_num; i++) {
		pthread_join(mgmt.phil_thread[i], NULL);
	}

	//Free Memory (Technically, it should never hit this point...)
	for (i = 0; i < mgmt.phil_num; i++)
		pthread_cond_destroy(&mgmt.cond[i]);
	free(pairarg);
	free(mgmt.phil);
	free(mgmt.chop);
	free(mgmt.phil_thread);
	free(mgmt.cond);

	//Have a nice day
	return;
}

