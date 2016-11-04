/*
 * COSC 361 - Project 2A: First Come, First Serve
 *
 * Description:
 *     Simulates the FCFS scheduling algorithm.
 *     Utilising CN_Vecs and File Streams, this program will read in from a
 *     file and gather data about processes. It will then take that
 *     information and calculate execute times, wait times, etc.
 *
 * Command Line Options:
 *     This program is multi-purpose. It can show a simulation, print out
 *     statistics, and even a PPM graph showing how the processes would be
 *     laid out in a real First Come, First Serve SScheduling System.
 *     Here are the options:
 *
 *     -i FILE_PATH
 *          Writes a binary PPM (P6) file to "FILE_PATH" of the simulation
 *
 *     -s
 *          Prints out the simulation for every second
 *
 *     -n
 *          Prevents printing out statistics (By default, they are printed)
 *
 *     -d
 *          Enables Debug Mode
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
#include "img_helper.h"

//CNDS (Clara Nguyen's Data Structures)
#include "lib/handy/cnds/cn_vec.h"
#include "lib/handy/cnds/cn_list.h"

typedef struct fcfs_process {
	    //Basic process elements
	int job_id,
	    arrival_time,
	    cpu_burst,
	    priority,

		//Conditonals
		sort_id,

		//Timing
		execute_time,
		wait_time,
	    turnaround_time;
} FCFS_PROCESS;

typedef struct snapshot {
	cn_byte *status;
} SNAPSHOT;

char* itos(int val) {
	char* buffer = (char *) malloc(sizeof(char) * 12);
	sprintf(buffer, "%d", val);
	return buffer;
}

main(int argc, char** argv) {
	cn_bool no_stats = CN_FALSE,
	        simulate = CN_FALSE,
	        printimg = CN_FALSE,
	        debugmod = CN_FALSE;
	char*   imgpath;

	//Initialise colours for grid
	PPM_RGB black_c, wait_c, cur_c;
	black_c.R = 0;
	black_c.G = 0;
	black_c.B = 0;

	wait_c.R = 222;
	wait_c.G = 222;
	wait_c.B = 222;

	cur_c.R  = 64;
	cur_c.G  = 64;
	cur_c.B  = 64;

	if (argc < 2) {
		//Clearly you have no idea what you are doing...
		fprintf(stderr, "Usage: fcfs job_file [-i IMG_PATH] [-s] [-n] [-d]\n");
		return -1;
	}
	if (!file_exists(argv[1])) {
		fprintf(stderr, "[ \e[1;31;mERROR\e[0m ] File doesn't exist.\n");
		return -2;
	}

	//Parse arguments
	cn_uint arg_index = 2;
	for (; arg_index < argc; arg_index++) {
		if (argv[arg_index][0] == '-') {
			switch (argv[arg_index][1]) {
				case 'i':
					//Image Export
					printimg = CN_TRUE;
					if (arg_index + 1 >= argc) {
						fprintf(stderr, "[ \e[1;31;mERROR\e[0m ] Wrong number of arguments proceeding \"-i\"\n");
						return -3;
					}
					imgpath = argv[++arg_index];
					break;
				case 's':
					simulate = CN_TRUE;
					break;
				case 'n':
					no_stats = CN_TRUE;
					break;
				case 'd':
					debugmod = CN_TRUE;
					break;
			}
		}
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

	cn_uint i = 1,
			pn = cn_vec_size(jobs);
	for (; i < pn; i++) {
		process = (FCFS_PROCESS *) cn_vec_at(jobs, i);
		process->execute_time    = prev_pr->execute_time + prev_pr->cpu_burst;
		process->wait_time       = process->execute_time - process->arrival_time;
		process->turnaround_time = process->cpu_burst    + process->wait_time;
		prev_pr = process;
	}

	double avg_wait       = 0,
	       avg_turnaround = 0,
	       total_time     = 0;
	for (i = 0; i < pn; i++) {
		process = (FCFS_PROCESS *) cn_vec_at(jobs, i);
		avg_wait       += process->wait_time;
		avg_turnaround += process->turnaround_time;
		total_time     += process->cpu_burst;
	}
	avg_wait       /= pn;
	avg_turnaround /= pn;

	//What if the user wanted a simulation?
	if (simulate == CN_TRUE || printimg == CN_TRUE) {
		char** job_names;
		job_names = (char **) malloc(sizeof(char *) * cn_vec_size(jobs) + 1);
		job_names[cn_vec_size(jobs)] = NULL;
		cn_uint nc = 0;
		for (; nc < cn_vec_size(jobs); nc++) {
			FCFS_PROCESS* proc = cn_vec_at(jobs, nc);
			job_names[nc] = itos(proc->job_id);
		}

		CN_LIST process_queue = cn_list_init(FCFS_PROCESS);
		CN_VEC snapshots   = cn_vec_init(SNAPSHOT);
		cn_uint time       = 0,                 //Timer Variable
				num_proc   = 0,                 //Number of processes executed
		        div_factor = cn_vec_size(jobs); //To divide total wait and turnaround times
		for (; time < total_time; time++) {
			cn_uint i = 0;
			FCFS_PROCESS* process;
			for (; i < cn_vec_size(jobs); i++) {
				process = cn_vec_at(jobs, i);
				if (process->arrival_time == time) {
					FCFS_PROCESS* temp = (FCFS_PROCESS *) malloc(sizeof(FCFS_PROCESS));
					memcpy(temp, process, sizeof(FCFS_PROCESS));
					temp->sort_id         = num_proc++;
					temp->wait_time       = 0;
					temp->turnaround_time = 0;
					cn_list_push_back(process_queue, temp);
					if (simulate == CN_TRUE)
						printf("[%d] Job #%d has been added to the queue (D: %d)\n", time, temp->job_id, temp->cpu_burst);
					cn_vec_delete(jobs, i);
					i--;
				}
			}

			if (!cn_list_empty(process_queue)) {
				//Get top element
				FCFS_PROCESS* cur_proc = cn_list_begin(process_queue)->data,
				            * new_proc;
				if (simulate == CN_TRUE)
					printf("[%d] Process Job \"%d\" (%d remaining)\n", time, cur_proc->job_id, cur_proc->cpu_burst - 1);
				cur_proc->cpu_burst--;

				//Make snapshot and set it equal to the number of original processes count
				SNAPSHOT snap;
				snap.status = (cn_byte *) malloc(sizeof(cn_byte) * pn);
				memset(snap.status, 0x00, sizeof(cn_byte) * pn);

				//Increment the waiting time of all other processes and modify snapshot
				CNL_ITERATOR ii;
				cn_list_traverse(process_queue, ii) {
					FCFS_PROCESS* iterate_process = ii->data;
					if (iterate_process != cur_proc) {
						//Only increment wait time if process isn't running
						iterate_process->wait_time++;
						snap.status[iterate_process->sort_id] = 1;
					} else {
						snap.status[iterate_process->sort_id] = 2;
					}

					//Increment turnaround time
					iterate_process->turnaround_time++;
				}
				cn_vec_push_back(snapshots, &snap); //Add the snapshot to the vector

				//Since CN_List frees elements when it pops them off, we must make
				//a copy of the data.
				if (cur_proc->cpu_burst <= 0) {
					//Add times
					printf("[%d] Process \"%d\" is done\n", time, cur_proc->job_id);
					cn_list_pop_front(process_queue);
				}
			}
		}

		if (printimg == CN_TRUE) {
			PPM img = ppm_init_size(16 + (24 * cn_vec_size(snapshots)), 28 + (8 * pn));
			FONT_SET font;
			font_set_load_font(&font, "data/font.bin");
			whiteout(img);

			//Draw title bar and bar
			font_write_on_ppm_centered((int)((double)img->width * 0.5), 2, &font, img, argv[1]);
			draw_rectangle_colour(img, 2, 10, img->width - 2, 11, &black_c);

			//Draw all process id's
			cn_uint prog = 0;
			for (; prog < div_factor; prog++) {
				font_write_on_ppm(4, 24 + (prog * 8), &font, img, job_names[prog]);
			}

			//Draw all bars and times
			cn_uint _a, _b;
			for (_b = 0; _b < cn_vec_size(snapshots); _b++) {
				char* time = itos(_b);
				font_write_on_ppm_centered(16 + (_b * 24), 14, &font, img, time);
				draw_rectangle_colour(img, 16 + (_b * 24), 22, 17 + (_b * 24), 25 + (div_factor * 8), &wait_c);
				free(time);
				SNAPSHOT* snap = cn_vec_at(snapshots, _b);
				for (_a = 0; _a < div_factor; _a++) {
					//printf("%d ", snap->status[_a]);
					cn_uint x1 = 16 + (_b * 24),
							y1 = 25 + (_a * 8),
							x2 = x1 + 24,
							y2 = y1 + 3;
					switch (snap->status[_a]) {
						case 0:
							//Blank
							break;
						case 1:
							//Waiting
							draw_rectangle_colour(img, x1, y1, x2, y2, &wait_c);
							break;
						case 2:
							//Running
							draw_rectangle_colour(img, x1, y1, x2, y2, &cur_c);
							break;
					}
				}
				//printf("\n");
				free(snap->status);
			}

			ppm_write_to_file(img, imgpath);
		}
	}

	//Free Memory
	cn_vec_free(jobs);

	///Print out
	printf("Average Wait Time      : %lg s\n"    , avg_wait);
	printf("Throughput (p/min)     : %lg p/min\n", 60 / (total_time / pn));
	printf("Average Turnaround Time: %lg s\n"    , avg_turnaround);
}
