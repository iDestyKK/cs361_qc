/*
 * COSC 361 - Project 2B: Round Robin
 *
 * Description:
 *     Simulates the Round Robin scheduling algorithm.
 *     Utilising CN_Queues and File Streams, this program will read in from a
 *     file and gather data about processes. It will then take that
 *     information and calculate execute times, wait times, etc.
 * 
 * Algorithm:
 *     The program will add all tasks to a vector, and then add them in a queue
 *     when the start time is reached. At each step, the resource at the top
 *     of the queue is read, and has its time subtracted. Then, if it is not 0,
 *     it will push it to the back of the queue.
 * 
 * Compilation:
 *     This program was written under the C GNU89 standard. As such, it should
 *     be compiled under the following command:
 *
 *     gcc --std=gnu89 -o fcfs fcfs.c lib/cn_vec/cn_vec.c .......
 * 
 * Author:
 *     Clara Van Nguyen
 */

//C includes
#include <stdio.h>
#include <stdlib.h>

//The secret weapon(s)
#include "lib/handy/handy.h"
#include "lib/handy/types.h"
#include "lib/handy/file.h"
#include "lib/handy/stream.h"

//CNDS [Clara Nguyen's Data Structures]
#include "lib/cn_vec/cn_vec.h"
#include "lib/handy/cnds/cn_list.h"

//Configure the RR Scheduler
const uint QUANTUM = 1;

typedef struct fcfs_process {
	    //Basic process elements
	int job_id,
	    arrival_time,
	    cpu_burst,
	    priority,

	    //RR Conditions
	    sort_id,

	    //Timing
	    execute_time,
	    wait_time,
	    turnaround_time;
} FCFS_PROCESS;

main(int argc, char** argv) {
	if (argc != 2) {
		//Clearly you have no idea what you are doing...
		fprintf(stderr, "Usage: rr job_file\n");
		return -1;
	}
	if (!file_exists(argv[1])) {
		fprintf(stderr, "[ \e[1;31;mERROR\e[0m ] File doesn't exist.\n");
		return -2;
	}

	//Initialise Vector and File Streams
	CN_VEC jobs = cn_vec_init(FCFS_PROCESS);
	FILE* fp = fopen(argv[1], "r");
	CN_FSTREAM fs = cn_fstream_init(fp);
	
	//Use the stream to read in until the end of the file
	cn_fstream_next(fs);
	while (cn_fstream_get(fs) != NULL) {
		//Welcome to the world of unreadable C
		FCFS_PROCESS process;
		int* data = (int*)&process;
		cn_uint i = 0;
		for (; i < 4; i++) {
			if (cn_fstream_get(fs) == NULL)
				break;
			data[i] = atoi(cn_fstream_get(fs)); //Wait, what!?
			cn_fstream_next(fs);
		}
		cn_vec_push_back(jobs, &process);
	}

	fclose(fp);
	cn_fstream_free(fs);
	
	//Make a queue to store the processes.
	CN_LIST process_queue = cn_list_init(FCFS_PROCESS);
	
	cn_uint time             = 0,                 //Timer Variable
	        quantum_shift    = 0,                 //Position in a time quantum
	        num_proc         = 0,                 //Number of processes executed
	        total_wait       = 0,                 //Total time of all processes spent waiting
	        total_turnaround = 0,                 //Total time of all processes turnaround
	        div_factor       = cn_vec_size(jobs); //To divide total wait and turnaround times

	cn_bool already_pop   = CN_FALSE;
	//Calculate execute and wait times.
	while (CN_TRUE) {
		//Check if any processes can be added at this time quantum.
		if (time == 0) {
			FCFS_PROCESS* process;
			cn_uint i    = 0,
			        at_i = 0;
			for (; i < cn_vec_size(jobs); i++) {
				process = cn_vec_at(jobs, i);
				if (process->arrival_time == time) {
					FCFS_PROCESS* temp = (FCFS_PROCESS *) malloc(sizeof(FCFS_PROCESS));
					memcpy(temp, process, sizeof(FCFS_PROCESS));
					temp->sort_id         = num_proc++;
					temp->wait_time       = 0;
					temp->turnaround_time = 0;
					cn_list_insert(process_queue, at_i++, temp);
					printf("[%d] Job #%d has been added to the queue (D: %d)\n", time, temp->job_id, temp->cpu_burst);
					cn_vec_delete(jobs, i);
					i--;
				}
			}
		}

		//Kill the loop if the queue and vector is empty.
		if (cn_list_empty(process_queue) && cn_vec_empty(jobs))
			break;
		
		if (!cn_list_empty(process_queue)) {
			//Get the top element.
			FCFS_PROCESS* cur_proc = cn_list_begin(process_queue)->data,
			            * new_proc;

			printf("[%d] Process Job \"%d\" (%d remaining)\n", time, cur_proc->job_id, cur_proc->cpu_burst - 1);
			cur_proc->cpu_burst--;

			//Increment the waiting time of all other processes
			CNL_ITERATOR ii;
			cn_list_traverse(process_queue, ii) {
				FCFS_PROCESS* iterate_process = ii->data;
				if (iterate_process != cur_proc) {
					//Only increment wait time if process isn't running
					iterate_process->wait_time++;
				}

				//Increment turnaround time
				iterate_process->turnaround_time++;
			}
			
			//Add a process if it arrives at the next time.
			FCFS_PROCESS* process;
			cn_uint i    = 0,
			        at_i = 0;
			for (; i < cn_vec_size(jobs); i++) {
				process = cn_vec_at(jobs, i);
				if (process->arrival_time == time + 1) {
					FCFS_PROCESS* temp = (FCFS_PROCESS *) malloc(sizeof(FCFS_PROCESS));
					memcpy(temp, process, sizeof(FCFS_PROCESS));
					temp->sort_id         = num_proc++;
					temp->wait_time       = 0;
					temp->turnaround_time = 0;
					cn_list_insert(process_queue, at_i++ + 1, temp);
					printf("[%d] Job #%d has been added to the queue (D: %d)\n", time + 1, temp->job_id, temp->cpu_burst);
					cn_vec_delete(jobs, i);
					i--;
				}
			}

			//Since CN_List frees elements when it pops them off, we must make
			//a copy of the data.
			if (QUANTUM > 1) {
				//Exclusive checks if the quantum is longer than "realtime".
				//Checks if the process finished before the RR Quantum ended
				if (cur_proc->cpu_burst == 0) {
					//cn_list_pop_front(process_queue);
					quantum_shift = QUANTUM; //Skip to next process automatically
					already_pop = CN_TRUE;
				}
			}
			if (quantum_shift + 1 >= QUANTUM || QUANTUM == 1) {
				if (cur_proc->cpu_burst > 0) {
					new_proc = (FCFS_PROCESS *) malloc(sizeof(FCFS_PROCESS));
					memcpy(new_proc, cur_proc, sizeof(FCFS_PROCESS));
					cn_list_push_back(process_queue, new_proc);
				} else {
					//Add times
					total_wait       += cur_proc->wait_time;
					total_turnaround += cur_proc->turnaround_time;
					printf("[%d] Process \"%d\" is done\n", time, cur_proc->job_id);
				}
		
				cn_list_pop_front(process_queue);
				quantum_shift = 0;
			}
			else
				quantum_shift++;
		}

		//Increment the time counter
		time++;
	}

	//At this point, all processes have been done. Calculate stats and print
	printf("Average Wait Time      : %lg s\n"    , (double)total_wait / div_factor);
	printf("Throughput (p/min)     : %lg p/min\n", 60. / (time / div_factor));
	printf("Average Turnaround Time: %lg s\n"    , (double)total_turnaround / div_factor);
}
