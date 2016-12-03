#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

struct job {
	job();
	int job_id,
	    arrival_time,
		cpu_burst,
		priority,

		execute_time,
		wait_time,
		turnaround_time;
};

job::job() {
	execute_time    = 0;
	wait_time       = 0;
	turnaround_time = 0;
}

int main(int argc, char* argv[]) {
	//Initialise variable
	vector<job> jobs;      //Jobs loaded but not executing
	vector<job> run_queue; //Jobs running

	unsigned int quantum = 3;
	
	//Read from the jobs file
	ifstream fp(argv[1]);
	if (fp.fail()) {
		fprintf(stderr, "Error: Unable to open file\n");
		return 1;
	}
	
	job temp;
	string a, b, c, d;
	while (fp >> a >> b >> c >> d) {
		temp.job_id       = atoi(a.c_str());
		temp.arrival_time = atoi(b.c_str());
		temp.cpu_burst    = atoi(c.c_str());
		temp.priority     = atoi(d.c_str());
		jobs.push_back(temp);
	}

	//Done reading
	fp.close();
	
	//Simulation time
	unsigned int time = 0;
	unsigned int quantum_remaining = quantum;
	double total_wait       = 0,
	       total_turnaround = 0,
	       num_proc         = jobs.size();
	while (true) {
		//Variables
		unsigned int num_read_in = 0;
		vector<job>::iterator insert_at = run_queue.begin();

		//Check if a process is to be executed at this time
		if (time == 0) {
			for (int i = 0; i < jobs.size(); i++) {
				if (jobs[i].arrival_time == time) {
					job temp;
					temp = jobs[i];
					if (run_queue.size() == 0 || num_read_in + 1 >= run_queue.size()) {
						run_queue.push_back(temp);
					} else {
						run_queue.insert(run_queue.begin() + num_read_in + 1, temp);
					}
					cout << "[" << time << "] " << "Job " << temp.job_id << " has been added to the queue (D: " << temp.cpu_burst << ")" << endl;
					jobs.erase(jobs.begin() + i);
					num_read_in++;
					i--; //Decrement for obvious reasons
				}
			}
		}

		if (jobs.size() == 0 && run_queue.size() == 0)
			break; //Done.

		//Adjust the times for the other jobs
		for (int i = 1; i < run_queue.size(); i++) {
			run_queue[i].turnaround_time++;
			run_queue[i].wait_time++;
		}

		//Oh and also check for processes here.
		for (int i = 0; i < jobs.size(); i++) {
			//We are going to add in processes that would appear ONE SECOND LATER. Just because
			//if a process's quantum is up, we want to go to the next object, which may appear
			//during the next CPU tick.
			if (jobs[i].arrival_time - 1 == time) {
				job temp;
				temp = jobs[i];
				if (run_queue.size() == 0 || num_read_in + 1 >= run_queue.size()) {
					run_queue.push_back(temp);
				} else {
					run_queue.insert(run_queue.begin() + num_read_in + 1, temp);
				}
				cout << "[" << time << "] " << "Job #" << temp.job_id << " has been added to the queue (D: " << temp.cpu_burst << ")" << endl;
				jobs.erase(jobs.begin() + i);
				num_read_in++;
				i--; //Decrement for obvious reasons
			}
		}


		//Modify the job's information... if the front job exists
		if (run_queue.size() > 0) {
			cout << "[" << time << "] Process Job \"" << run_queue[0].job_id << "\" (" << run_queue[0].cpu_burst << " remaining)" << endl;
			run_queue[0].turnaround_time++;
			run_queue[0].cpu_burst--;

			if (run_queue[0].cpu_burst == 0) {
				//This process is done
				total_wait       += run_queue[0].wait_time;
				total_turnaround += run_queue[0].turnaround_time;
				cout << "[" << time << "] Job " << run_queue[0].job_id << " is done. (W: " << run_queue[0].wait_time << ", T: " << run_queue[0].turnaround_time << ")" << endl;
				run_queue.erase(run_queue.begin());
				quantum_remaining = quantum;
			}
			else {
				quantum_remaining--;
				if (quantum_remaining == 0) {
					quantum_remaining = quantum;
					run_queue.push_back(run_queue[0]);
					run_queue.erase(run_queue.begin());
				}
			}
		}

		//Increment time
		time++;
	}

	cout << "Average Wait      : " << (total_wait       / num_proc)  << endl;
	cout << "Throughput (p/min): " << (60. / (time      / num_proc)) << endl;
	cout << "Average Turnaround: " << (total_turnaround / num_proc)  << endl;
}
