# Programming Assignment 2

Programming Assignment 2 consists of 3 primary parts and 1 extra credit section. These all revolve around simulating scheduling algorithms. The order of these programs go as follows: FCFS (First Come, First Serve), RR (Round Robin), SJF (Shortest Job First), Priority Based Scheduling.

## General Information about all programs
All of these programs take similar arguments and do similar things. The only difference is the algorithm that they use. Each program takes a text file that contains job information and prints out statistics like total wait time and total turnaround time. It also allows the user to generate a PPM image showing what is actually going on. Here are the parameters you may use:

+ **-i FILE_PATH** - Writes a binary PPM (P6) file to specified "FILE\_PATH" visually showing the simulation.
+ **-s** - Prints out the simulation details for every second to stdout.
+ **-n** - Prevents printing out statistics (By default, statistics are printed).
+ **-d** - Enables Debug Mode. For the programs, this means that the file read in is echoed back to stdout to make sure it was read correctly.

The following commands are exclusive to some programs.

+ **-x** - Forces non-preemptive process execution (**SJF and Priority only**)
+ **-q QUANTUM** - Tells the program how long the time quantum is. Must be 1 or higher. (**RR only**)

## Part 1 - FCFS
FCFS stands for "First Come, First Serve", and it literally does just that. If jobs are thrown in, they will be executed in the order that they come in. This is done with the CN\_LIST and CN\_VEC libraries. All jobs are stored into a CN\_VEC, which are then thrown into a CN\_LIST. The CN\_LIST acts like a queue, except values other than the top can be accessed. When a process comes in, it is thrown into the queue in a certain position depending on how many other processes were added in. For instance, if process 1 was running and process 2 and 3 come in at the same time, process 2 will be next in the queue, while process 3 will be after process 2. All processes will be popped from the front when their CPU burst is over.

### Synopsis
./fcfs job\_file \[-i IMG\_PATH\] \[-s\] \[-n\] \[-d\]

### Example
Execute and print out statistics for "jobs.txt" using FCFS

**Command** ./fcfs jobs.txt

Execute and print out statistics for "jobs.txt". Also show the process in "img.ppm" and print out the simulation to stdout.

**Command** ./fcfs jobs.txt -s -i img.ppm

## Part 2 - RR
RR stands for "Round Robin" and it is probably the most complex (but most fair) of the three main parts of this assignment. We are given a time quantum "N" (Default: 1) and we have to cycle through all waiting processes in a queue that number of times. So every process gets to run at most "N" seconds before having to switch control to the next process in the queue. If a process completes early in its quantum, it will switch to the next process instantly and give it full quantum time. Just like FCFS, when a process's CPU burst ends, it will be popped off the queue.

### Synopsis
./rr job\_file \[-q QUANTUM\] \[-i IMG\_PATH\] \[-s\] \[-n\] \[-d\]

### Example
Execute and print out statistics for "jobs.txt" using RR

**Command** ./rr jobs.txt

Execute and print out statistics for "jobs.txt". Also show the process in "img.ppm" and print out the simulation to stdout.

**Command**: ./rr jobs.txt -s -i img.ppm

Set time quantum to 3, execute, and print out statistics for "jobs.txt"

**Command** ./rr jobs.txt -q 3

## Part 3 - SJF (SJN)
SJF is known as "Shortest Job First". However, it is also known as "Shortest Job Next". This one actually implements two versions which is determined by the **-x** command line argument. Without the argument, the algorithm is **preemptive**. Otherwise, it isn't. What does this mean? It acts like FCFS, but the moment a job comes in with a shorter CPU burst, control is instantly switched over to that job. If it isn't preemptive, it will be like FCFS but sorted by CPU burst time. If a process hasn't arrived yet, it will not be considered in the sort since it won't be in the queue.

### Synopsis
./sjf job\_file \[-i IMG\_PATH\] \[-x\] \[-s\] \[-n\] \[-d\]

### Example
Execute and print out statistics for "jobs.txt" using SJF (Preemptive)

**Command** ./sjf "jobs.txt"

Execute the same as above except without being preemptive

**Command** ./sjf "jobs.txt" -x

Execute same as above, and show the process in "img.ppm"

**Command** ./sjf "jobs.txt" -x -i img.ppm

## Part 4 - Priority
This one implements preemptive priority based scheduling. Just like SJF, the **-x** command line argument will determine if the algorithm is preemptive or not. Without the argument, the algorithm is **preemptive**. Otherwise, it isn't. What does this mean? It acts like FCFS, but the moment a job comes in with a higher priority (lower number), control is instantly switched over to that job. If it isn't preemptive, it will be like FCFS but sorted by priority. If a process hasn't arrived yet, it will not be considered in the sort since it won't be in the queue.

### Synopsis
./priority job\_file \[-i IMG\_PATH\] \[-x\] \[-s\] \[-n\] \[-d\]

### Example
Execute and print out statistics for "jobs.txt" using priority (Preemptive)

**Command** ./priority "jobs.txt"

Execute the same as above except without being preemptive

**Command** ./priority "jobs.txt" -x

Execute same as above, and show the process in "img.ppm"

**Command** ./priority "jobs.txt" -x -i img.ppm

## Output (For all programs)
Printing out statistics only.
```
Average Wait Time      : 3.8 s
Throughput (p/min)     : 15 p/min
Average Turnaround Time: 7.8 s
```

Printing out simulation.
```
[0] Job #1 has been added to the queue (D: 2)
[0] Process Job "1" (1 remaining)
[1] Job #2 has been added to the queue (D: 1)
[1] Process Job "1" (0 remaining)
[1] Process "1" is done
[2] Job #3 has been added to the queue (D: 8)
[2] Process Job "2" (0 remaining)
[2] Process "2" is done
[3] Process Job "3" (7 remaining)
[4] Job #4 has been added to the queue (D: 4)
[4] Process Job "3" (6 remaining)
[5] Job #5 has been added to the queue (D: 5)
[5] Process Job "3" (5 remaining)
[6] Process Job "3" (4 remaining)
[7] Process Job "3" (3 remaining)
[8] Process Job "3" (2 remaining)
[9] Process Job "3" (1 remaining)
[10] Process Job "3" (0 remaining)
[10] Process "3" is done
[11] Process Job "4" (3 remaining)
[12] Process Job "4" (2 remaining)
[13] Process Job "4" (1 remaining)
[14] Process Job "4" (0 remaining)
[14] Process "4" is done
[15] Process Job "5" (4 remaining)
[16] Process Job "5" (3 remaining)
[17] Process Job "5" (2 remaining)
[18] Process Job "5" (1 remaining)
[19] Process Job "5" (0 remaining)
[19] Process "5" is done
```

Image of simulation

![image not loaded](http://i.imgur.com/joxguPb.png "SJF")
