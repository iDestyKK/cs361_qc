# Programming Assignment 3

Programming Assignment 3 consists of a single part, unlike the last two. This program does what the name implies, simulates a disk (Well, a Hard Drive). The simulation was written in C and compiles under the GNU89 Standard.

## Program Information
The program only takes one argument, and that is a text file that is used to simulate files being either added, modified, or removed from the file system. It should be noted that files are not actually created, only simulated. The virtual disk has 10 MB (1024 * 1024 * 10 bytes) with contain sectors of size 512 bytes. As a result, there are 20480 total sectors to work with.

### Compilation
This application comes with a makefile which can be used to either compile or remove the program.

To compile disksim:
```
make
```

To remove disksim:
```
make clean
```

### How it works
Please read the proj3.pdf for full documentation on how it works.

### Notes
The program defines the disk attributes in "\#define" tags. You can change this to make more space available or to make sectors larger. Also, you can enable DEBUG mode by modifying the defines as. I decided not to include these as command line arguments since they are not meant to be changed by the end-user.

### Synopsis
./disksim file

### Output

**Command** ./disksim file\_operations.txt

```
Allocated Sectors     : 40
Total Sectors         : 20480
Disk Utilisation Ratio: 0.00195312%
Number of Files       : 7
Free Space            : 9.98047 MB (10465280 bytes)
```
