/*
 * COSC 361 - Project 2A: First Come First Serve
 *
 * Description:
 *     Simulates the FCFS scheduling algorithm.
 *     Utilising CN_Vecs and File Streams, this program will read in from a
 *     file and gather data about processes. It will then take that
 *     information and calculate execute times, wait times, etc.
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
#include "lib/cn_vec/cn_vec.h"

typedef struct fcfs_process {
	    //Basic process elements
	int job_id,
	    arrival_time,
	    cpu_burst,
	    priority,

		//Timing
		execute_time,
		wait_time,
	    turnaround_time;
} FCFS_PROCESS;

main(int argc, char** argv) {
	if (argc != 2) {
		//Clearly you have no idea what you are doing...
		fprintf(stderr, "Usage: fcfs job_file\n");
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
	
	//Calculate execute and wait times.
	FCFS_PROCESS* process = (FCFS_PROCESS *) cn_vec_at(jobs, 0),
	            * prev_pr = NULL;
	process->execute_time = 0;
	process->wait_time = 0;
	process->turnaround_time = process->cpu_burst + process->wait_time;
	prev_pr = process;
	
	cn_uint i = 1;
	for (; i < cn_vec_size(jobs); i++) {
		process = (FCFS_PROCESS *) cn_vec_at(jobs, i);
		process->execute_time    = prev_pr->execute_time + prev_pr->cpu_burst;
		process->wait_time       = process->execute_time - process->arrival_time;
		process->turnaround_time = process->cpu_burst    + process->wait_time;
		prev_pr = process;
	}
	
	double avg_wait       = 0,
	       avg_turnaround = 0,
	       total_time     = 0;
	for (i = 0; i < cn_vec_size(jobs); i++) {
		process = (FCFS_PROCESS *) cn_vec_at(jobs, i);
		avg_wait       += process->wait_time;
		avg_turnaround += process->turnaround_time;
		total_time     += process->cpu_burst;
		/*printf("Job #%d\n    Arrival: %d\n    CPU Boost: %d\n    Priority: %d\n    Wait Time: %d\n",
		       process->job_id,
		       process->arrival_time,
		       process->cpu_burst,
		       process->priority,
		       process->wait_time
		);*/
	}
	avg_wait       /= cn_vec_size(jobs);
	avg_turnaround /= cn_vec_size(jobs);

	printf("Average Wait Time      : %lg s\n"    , avg_wait);
	printf("Throughput (p/min)     : %lg p/min\n", 60 / (total_time / cn_vec_size(jobs)));
	printf("Average Turnaround Time: %lg s\n"    , avg_turnaround);

	//Free Memory
	cn_vec_free(jobs);
}
