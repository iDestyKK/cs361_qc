/*
 * COSC 361 - Project 3X: Concept
 *
 * Description:
 *     This code is simply for trying out the concept of simulating a file
 *     system. It is not expected to match the algorithms in the textbook but
 *     it is enough to suffice.
 * 
 * Author:
 *     Clara Van Nguyen
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HDD_SIZE  10000000
#define SECT_SIZE 512

typedef unsigned int cn_uint;

typedef struct disk {
	char*  data;
	char** sector;
} DISK;

main() {
	printf("Initialising... ");
	DISK disk;
	disk.data   = (char  *) malloc(sizeof(char ) * HDD_SIZE);
	disk.sector = (char **) malloc(sizeof(char*) * (HDD_SIZE / SECT_SIZE));
	cn_uint i = 0;
	for (; i < HDD_SIZE / SECT_SIZE; i++) {
		disk.sector[i] = disk.data + (i * SECT_SIZE);
	}
	printf("Done.\n");
	printf("System Memory: %d\nSector Size: %d\nNumber of Sectors: %d\n", HDD_SIZE, SECT_SIZE, HDD_SIZE / SECT_SIZE);
}
