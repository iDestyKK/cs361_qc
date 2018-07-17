COSC 361 - Programming Assignment 2

INFORMATION
    Contains programs fcfs, rr, sjf, and priority for simulating scheduling
    algorithms. These are written in C under the GNU89 standard and are all
    structured very similarly to each other, using Clara Nguyen's libraries
    for data structures such as CN_List, CN_Vec, and Handy.

COMPILATION
    Run "make" and let it do the compilation for you on Tesla or Hydra.

FCFS INFORMATION
    Synopsis: ./fcfs job_file [-s] [-n] [-d] [-i IMG_PATH]

    Simulates the "First Come, First Serve" algorithm. This program doesn't
    actually do any simulations unless the "-s" parameter is specified.
    Without it, it can simply perform FCFS calculations by loading all of
    the data from the jobs file into an array and calculating via differences
    in time.

    To load the file in, we utilise my CN_VEC library and Handy's CN_FSTREAMs
    to read in the file as strings, and then convert them to integers which
    then stored into an array of structs. We then for loop around it and
    calculate results.

    EXAMPLE USAGES
        To run and get job times for "jobs.txt", type the following:
            ./fcfs jobs.txt
	
        To run, print simulation, and get job times for "jobs.txt":
            ./fcfs jobs.txt -s

        To run, and output a PPM file showing the simulation to "img.ppm":
            ./fcfs jobs.txt -i img.ppm

RR INFORMATION
    Synopsis: ./rr job_file [-q QUANTUM] [-s] [-n] [-d] [-i IMG_PATH]
    
    This program is written in a similar fashion to FCFS, except it actually
    performs the simulation to get its results. To do this, it copies all
    jobs into a CN_LIST, which it can use to push and pop from either side
    in constant time. It then acts like a queue, running the job at the front
	and cycling it around after a certain time quantum.

    This program's arguments are the exact same as FCFS's, except it adds a
    new parameter "-q QUANTUM", which allows the end-user to specify the
    time quantum a job gets to execute before switching to the next one. If
    "-q" is unspecified, the time quantum is "1" by default.

    EXAMPLE USAGES
        To run and get job times for "jobs.txt", type the following:
            ./rr jobs.txt

        To run, print simulation, and get job times for "jobs.txt":
            ./rr jobs.txt -s

        To run the same as above, with a time quantum of 3:
            ./rr jobs.txt -s -q 3

        To output a PPM file "img.ppm" of the above:
            ./rr jobs.txt -s -q 3 -i img.ppm

SJF INFORMATION
    Synopsis: ./sjf job_file [-x] [-s] [-n] [-d] [-i IMG_PATH]

    This program is written in a similar fashion to RR, except it has two modes
    (primarily due to a student saying it was supposed to be "preemptive"). The
    program implements preemptive and non-preemptive SJF which can be determined
    by the "-x" argument. If "-x" is present, the program performs non-preemptive
    SJF. If it is absent, it will perform preemptive SJF.

    SJF was achieved here by utilising C's qsort() function and sorting based on
    remaining job times. When non-preemptive, the job will continue until done.
    This effect was achieved by sorting every value "except" for the front one. If
    preemptive, the entire array of jobs gets sorted.

    EXAMPLE USAGES
        To run preemptively and get job times for "jobs.txt":
            ./sjf jobs.txt

        To do the same as above but non-preemptively:
            ./sjf jobs.txt -x

        To print out a PPM file "img.ppm" of above:
            ./sjf jobs.txt -x -i "img.ppm"

PRIORITY INFORMATION
    Synopsis: ./priority job_file [-x] [-s] [-n] [-d] [-i IMG_PATH]

    This program was copied and pasted from sjf.c and had its sort function
    modified to work with priority, because they are closely related. Just like
    SJF, this program implements the "-x" argument, which determines the algorithm
    used. If "-x" is present, the algorithm is "non-preemptive". Otherwise, it is
    "preemptive".

    Because this was copied from sjf.c, the algorithm is the exact same. The only
    thing that has changed is the sort comparison function, which compares priority
    as opposed to CPU burst times.

    EXAMPLE USAGES
        To run preemptively and get job times for "jobs.txt":
            ./priority jobs.txt

        To do the same as above but non-preemptively:
            ./priority jobs.txt -x

        To print out a PPM file "img.ppm" of above:
            ./priority jobs.txt -x

MISCELLANEOUS
    Parameter information shared by all programs:
        -s - "Simulate"
             Prints out a simulation of the scheduling algorithm to stdout.

        -n - "No Times"
             Prevents printing out time data. (By default, it will print)

        -d - "Debug Mode"
             Prints out each line read in via CN_FSTREAM to make sure it is
             read in correctly.

        -i - "PPM Output Path"
             Expects the next argument to contain the path to a PPM file,
             which the program will write bytes to.

DISCLAIMER
    I, Clara Nguyen, have gotten permission from Dr. Cao to utilise my libraries
    in these programming assignments as long as I have written them myself. This
    includes all libraries in the "lib" folder for this project.
