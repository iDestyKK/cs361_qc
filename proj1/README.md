# Programming Assignment 1

Programming Assignment 1 consists of three parts. Sudoku Puzzle Validator, Dining Philosophers, and a text converter/parser.
These assignments are to practise the behaviours of programming with pthreads in C.

## Part 1 - Sudoku Validator
Runs 3 threads (as opposed to the suggested 11), one for rows, one for columns, and one for a 3x3 grid. Accepts a ".su" file, which contains width, height, and then sudoku puzzle data separated out with spaces or new lines. The algorithm uses the first 9 bits of shorts (16-bit integers) and sets bits to "1" for each number that appears in each row, column, or grid respectively. If each function returns 0x1FF (511) every time, the sudoku puzzle is guaranteed to be correct.

This program utilises the CN\_Grids Data Structure Library (written by Clara) to aid in storing information about the sudoku puzzle.

### Synopsis
./validator file

### Example
**Command** ./validator 1\_sudoku\_validator/puzzle1.su
```
1
```

## Part 2 - Dining Philosophers
The classic Computer Science problem featuring philosophers sitting at a table with an unlimited supply of noodles (or spaghetti?). The twist in the problem comes with the number of chopsticks being equal to the number of philosophers at the table (e.g. 5 philosophers means 5 chopsticks are present). Aside from the lack of washing these chopsticks between use, a philosopher can only eat when they have two chopsticks. The goal of this problem is to demonstrate avoiding deadlocks. For instance, if all of the philosophers tried picking up their right chopstick, they would never be able to pick up their left one, resulting in a deadlock. This program solves the problem by adding the following rules/conditions to it:

+ Each philosopher has a thread of its own. All of these threads share access to a "manager" struct and can only modify its data if it locks a mutex.
+ When a philosopher picks up one chopstick, it will not stop trying to access the other until it is freed. Starvation is prevented because the philosopher is guaranteed to let go of the chopsticks after a certain period of time.
+ If the philosopher's ID is an even number, it picks its right chopstick up first, then its left. Otherwise, flip the order.
+ The max number of philosophers who can pick up a chopstick is limited to PHILOSOPHER\_NUM - 1 (e.g. If there are 5 philosophers, only 4 can hold a chopstick at once).

**Note**: This program utilises CN data types such as cn\_uint (unsigned int) and cn\_bool (C++ bool) which are included in "lib/handy/types.h".

**Note**: There are two .c files. "philosophers.c" is the assignment, and the makefile will target this one to compile. "philosophers\_orig.c" is an alternative version which features command line arguments where you can specify the number of philosophers, speed of eating/thinking, etc.

### Synopsis
./philosophers

### Example
**Command**: ./philosophers
```
Philosopher 0 has joined.
Philosopher 0 wants to eat.
Philosopher 0 is eating for 2.04996 seconds.
Philosopher 1 has joined.
Philosopher 1 is thinking for 2.03838 seconds.
Philosopher 3 has joined.
Philosopher 3 is thinking for 2.47537 seconds.
Philosopher 2 has joined.
Philosopher 2 wants to eat.
Philosopher 2 is eating for 2.46172 seconds.
...
```

## Part 3 - Text Parser
Runs 4 threads. Each thread runs a different task in this program. Each thread has its own CN\_Queue (except for the fourth), which stores pointers to char\*'s. The program, as a whole, will read a text file of the user's choice, and convert all spaces to asterisks (\*), and then convert all lower case letters to upper case letters, then print them out to either stdout or a file.

Each thread works as follows:

**Thread 1** - Reads all lines into a queue.

**Thread 2** - Takes strings from the queue created in Thread 1, converts all spaces to asterisks, and then puts them in a new queue.

**Thread 3** - Takes strings from the queue created in Thread 2, converts all lower case characters to upper case, and then puts them in a new queue.

**Thread 4** - Takes strings from the queue created in Thread 3, and outputs them to either stdout or a file depending on what a user specified when running the program.

### Synopsis
./parser file [output\_file]

### Example
**Command**: ./parser 3\_text\_parser/txt1

**Input**
```
This is a test.
I didn't want to believe it.
but it is so.
```

**Output**
```
THIS*IS*A*TEST.
I*DIDN'T*WANT*TO*BELIEVE*IT.
BUT*IT*IS*SO.
```
