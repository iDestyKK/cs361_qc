/*
 * COSC 361 - Project 3: Disk Simulator
 *
 * Descritpion:
 *     Simulates usage of a disk, or a memory drive which stores files and
 *     other data. The purpose of this is to understand how a hard drive
 *     stores data. However, this will not be random access. It will be linear
 *     as in files will be stored from the start of the space and go toward
 *     the end.
 *
 *     The data is is stored in a modified version of a CN_LIST data structure
 *     which also stores pointers to the next sector that holds data. Since
 *     a file may (and probably will be) larger than a block, it needs to
 *     point to the next block which stores data from it.
 *
 *     Data is read in via a file stream. It is a library I wrote which allows
 *     a user to traverse a plain text file and extract data separated by
 *     spaces. This is called a CN_FSTREAM.
 * 
 * Author:
 *     Clara Van Nguyen
 */

#include <stdio.h>
#include <stdlib.h>

//The secret weapon(s)
#include "lib/handy/handy.h"
#include "lib/handy/types.h"
#include "lib/handy/file.h"
#include "lib/handy/stream.h"

//CNDS (Clara Nguyen's Data Structures)
#include "lib/handy/cnds/cn_vec.h"

//Information on the disk
#define HDD_SIZE  10485760 //10 MB
#define SECT_SIZE 512      //.5 KB

//Program Variables
#define DEBUG 0

//Sector Struct
typedef struct sector {
	struct sector* prev,      //Pointer to previous sector (If allocated)
	             * next;      //Pointer to next sector     (If allocated)
	cn_bool        allocated; //Whether this sector is allocated or not
	struct vfile * file;      //File this is allocated to
} SECTOR;

//Disk Struct
typedef struct disk {
	CN_VEC         sectors,   //Sectors vector
	               files;     //File vector
	cn_uint        allocated, //Allocated sectors
	               total;     //Total sectors
} DISK;

//Virtual File Struct
typedef struct vfile {
	struct sector* start;     //Sector which has the start to this file
	cn_uint        id,        //ID of file
	               size;      //Size of file
} VFILE;

//Global Vector to allocate the memory in
DISK sim;

//API Functions
void allocate(int filehandle, int filesize) {
	//printf("%d, %d\n", filehandle, filesize);
	//Traverse the disk to find a sector that is free
	SECTOR* data = cn_vec_data(sim.sectors);
	cn_uint i = 0;
	for (; i < sim.total; i++) {
		if (data[i].allocated == CN_FALSE)
			break;
	}

	if (i == sim.total) {
		//Can't do it.
		fprintf(stderr, "[ ERROR ] Can not allocate file (out of space).\n");
		return;
	}

	//Guaranteed allocation
	//Allocate the file vector
	VFILE  tmp,
	     * file;
	memset(&tmp, 0, sizeof(struct vfile));
	cn_vec_push_back(sim.files, &tmp);
	file = (VFILE *) cn_vec_at(sim.files, cn_vec_size(sim.files) - 1);
	
	file->start = &data[i];
	file->size  = filesize;
	file->id    = filehandle;
	SECTOR* prev = NULL;
	cn_uint fsz = filesize;
	while (fsz != 0) {
		//Hachish for-loop
		for (; data[i].allocated == CN_TRUE; i++);
		if (DEBUG)
			printf("Allocated Sector #%d for file %d\n", i, file->id);
		
		data[i].prev      = prev;
		if (prev != NULL) {
			prev->next    = &data[i];
		}
		data[i].allocated = CN_TRUE;
		data[i].file      = file;
		prev              = &data[i];
		sim.allocated++;
		
		if (fsz < SECT_SIZE)
			fsz = 0;
		else
			fsz -= SECT_SIZE;
	}
}

void modify(int filehandle, int newfilesize) {
	//Find the entry in the file table
	cn_uint i = 0;
	SECTOR* data = cn_vec_data(sim.sectors);
	VFILE* fptr;
	for (; i < cn_vec_size(sim.files); i++) {
		fptr = (VFILE *) cn_vec_at(sim.files, i);
		if (fptr->id == filehandle)
			break;
	}
	if (i >= cn_vec_size(sim.files))
		return; //Invalid file ID
	
	//Don't bother with the rest if the filesizes are the same
	if (newfilesize == fptr->size)
		return;

	//Now go and find the last sector for that file.
	SECTOR* sptr = fptr->start;
	while (sptr->next != NULL)
		sptr = sptr->next;
	
	//Next action depends on whether the new size is larger or smaller
	if (newfilesize > fptr->size) {
		//New size is larger
		cn_uint szmod = newfilesize - (((fptr->size / SECT_SIZE) * SECT_SIZE) + ((fptr->size % SECT_SIZE > 0) ? SECT_SIZE : 0));
		if (DEBUG)
			printf("Sector Difference: %d\n", szmod);
		fptr->size = newfilesize;
		if (szmod == 0)
			//If the current file could fit into the sectors from earlier, kill it
			return;
		
		i = 0;
		SECTOR* prev = sptr;
		while (szmod != 0) {
			//Append sectors.
			for (; data[i].allocated == CN_TRUE; i++);
			if (DEBUG)
				printf("Allocated Sector #%d for file %d\n", i, fptr->id);
			prev->next        = &data[i];
			data[i].prev      = prev;
			data[i].allocated = CN_TRUE;
			data[i].file      = fptr;
			prev              = &data[i];
			sim.allocated++;
			
			if (szmod < SECT_SIZE)
				szmod = 0;
			else
				szmod -= SECT_SIZE;
		}
	}
	else {
		//New size is smaller
		cn_uint szmod = ((newfilesize / SECT_SIZE) * SECT_SIZE) + ((newfilesize % SECT_SIZE > 0) ? SECT_SIZE : 0),
		        fmod  = ((fptr->size  / SECT_SIZE) * SECT_SIZE) + ((fptr->size  % SECT_SIZE > 0) ? SECT_SIZE : 0);
		//Go to the end of the file and start deleting from the back.
		SECTOR* prev = sptr;
		cn_uint sect_num = 1;
		while (prev->next != NULL) {
			sect_num++;
			prev = prev->next;
		}

		while (fmod != szmod) {
			if (DEBUG)
				printf("Unallocated sector #%d\n", ((char *)prev - (char *)data) / sizeof(SECTOR));
			prev                  = prev->prev; //Yes
			prev->next->allocated = CN_FALSE;
			prev->next->prev      = NULL;
			prev->next->next      = NULL;
			prev->next->file      = NULL;
			prev->next            = NULL;
			fmod -= SECT_SIZE;
			sim.allocated--;
		}
		fptr->size = newfilesize;
	}
}

void release(int filehandle) {
	if (DEBUG)
		printf("%d\n", filehandle);
	//Find the entry in the file table
	cn_uint i = 0, ip;
	SECTOR* data = cn_vec_data(sim.sectors);
	VFILE* fptr;
	for (; i < cn_vec_size(sim.files); i++) {
		fptr = (VFILE *) cn_vec_at(sim.files, i);
		if (fptr->id == filehandle)
			break;
	}
	ip = i;
	if (i >= cn_vec_size(sim.files))
		return; //Invalid file ID
	
	//Now go and find the last sector for that file.
	SECTOR* sptr = fptr->start, *next;
	while (sptr != NULL) {
		if (DEBUG)
			printf("Unallocated sector #%d\n", ((char *)sptr - (char *)data) / sizeof(SECTOR));
		next = sptr->next;

		//Now remove the previous one
		sptr->next      = NULL;
		sptr->prev      = NULL;
		sptr->allocated = CN_FALSE;
		sptr->file      = NULL;
		sim.allocated--;

		sptr = next;
	}
	cn_vec_delete(sim.files, ip);
}

void print_allocation_map() {
	cn_uint i = 0;
	SECTOR* data = cn_vec_data(sim.sectors);
	for (; i < cn_vec_size(sim.sectors); i++) {
		if (data[i].file != NULL) {
			printf("%d - %d\n", i, data[i].file->id);
		}
		else
			printf("%d - NULL\n", i);
	}
}

main(int argc, char** argv) {
	//Parse Arguments
	if (argc != 2) {
		//Clearly you have no idea what you're doing
		fprintf(stderr, "Usage: disksim file\n");
		return -1;
	}

	//Set up the disk
	sim.files     = cn_vec_init(VFILE);
	sim.sectors   = cn_vec_init(SECTOR);
	cn_vec_resize(sim.sectors, HDD_SIZE / SECT_SIZE);
	sim.allocated = 0;
	sim.total     = cn_vec_size(sim.sectors);

	//Set all sectors to "not allocated"
	cn_uint i = 0;
	for (; i < cn_vec_size(sim.sectors); i++) {
		cn_vec_get(sim.sectors, SECTOR, i).prev      = NULL;
		cn_vec_get(sim.sectors, SECTOR, i).next      = NULL;
		cn_vec_get(sim.sectors, SECTOR, i).allocated = CN_FALSE;
	}
	
	//Check if the file in question even exists. If not, yell about it.
	if (!file_exists(argv[1])) {
		fprintf(stderr, "[ \e[1;31;mERROR\e[0m ] File doesn't exist.\n");
		return -2;
	}

	//Open up file and file stream
	FILE* fp = fopen(argv[1], "r");
	CN_FSTREAM fs = cn_fstream_init(fp);
	cn_fstream_next(fs);
	
	//Begin Parsing
	while (cn_fstream_get(fs) != NULL) {
		int fileid, filesize;

		//This command is guaranteed to be a char (A, R, or M).
		if (DEBUG)
			printf("%c\n", cn_fstream_get(fs)[0]);
		switch (cn_fstream_get(fs)[0]) {
			case 'A':
				//Allocate
				cn_fstream_next(fs); fileid   = atoi(cn_fstream_get(fs));
				cn_fstream_next(fs); filesize = atoi(cn_fstream_get(fs));

				allocate(fileid, filesize);
				break;
			case 'R':
				//Release
				cn_fstream_next(fs); fileid   = atoi(cn_fstream_get(fs));

				release(fileid);
				break;
			case 'M':
				//Modify
				cn_fstream_next(fs); fileid   = atoi(cn_fstream_get(fs));
				cn_fstream_next(fs); filesize = atoi(cn_fstream_get(fs));

				modify(fileid, filesize);
				break;
		}
		cn_fstream_next(fs);
	}

	fclose(fp);

	//print_allocation_map();
	printf("Allocated Sectors     : %d\nTotal Sectors         : %d\nDisk Utilisation Ratio: %g%\nNumber of Files       : %d\nFree Space            : %g MB (%d bytes)\n", sim.allocated, sim.total, (double)sim.allocated / sim.total, cn_vec_size(sim.files), ((double)(sim.total - sim.allocated) * SECT_SIZE) / 1048576, (sim.total - sim.allocated) * SECT_SIZE);
}
