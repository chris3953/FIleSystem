/**************************************************************
* Class:  CSC-415-0# Fall 2021
* Names: 
* Student IDs:
* GitHub Name:
* Group Name:
* Project: Basic File System
*
* File: fsInit.c
*
* Description: Main driver for file system assignment.
*
* This file is where you will start and initialize your system
*
**************************************************************/


#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "fsInit.h"
#include "fsLow.h"
#include "mfs.h"
#include "FreeSpace.c"
#include "FreeSpace.h"

#define SIGNATURE 0xBEEFED
#define NUM_ENTRIES 51
#define DIR_ENTRY_SIZE 40
#define ENTRY_MEM NUM_ENTRIES * DIR_ENTRY_SIZE
#define D_ENTRY_BLOCKS 4 


 VCB* vcb = NULL;
 int initRootDirectory(VCB* vcb); 

int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize)
	{
	printf ("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);
	/* TODO: Add any code you need to initialize your file system. */
	
	vcb = malloc(blockSize);
	LBAread(vcb, 1, 0); //read one block, start from block 0		
	
	// if signature matches, then it is initialized
	if( vcb->signature == SIGNATURE){
		free(vcb);
		vcb = NULL;
		printf("Signature matched, Already initialized");
		return -1; //ERROR		
	} else {
		// initialize VCB
		//vcb->root=;
		vcb->totalBlocks = numberOfBlocks;
		vcb->block_size = blockSize;
		vcb->free_space_start_block = 0;
		vcb->signature = SIGNATURE;
		initRootDirectory(vcb); 
		LBAwrite(vcb,1,0); 
	}
	return 0;
	}
	

int initRootDirectory(VCB* vcb){
	int startBlock = GetFreeSpace(D_ENTRY_BLOCKS);  //need to define D_ENTRY_BLOCKS

	time_t t;
	time(&t);

	vcb->root = startBlock;  //set root block number

	// Create directory entries for "." and ".."
    DirectoryEntry* de = (DirectoryEntry*) malloc(sizeof(DirectoryEntry)); // Create a new directory entry
	if(!de){
		printf("Memory allocation error.\n");
		return -1;
	}

    strcpy(de->name, ".");
    de->size = ENTRY_MEM;
    de->location[0].size = startBlock;
    de->creation_date = t;
    de->last_modified = t;
	de->last_opened = t;
    de->type = 1; 

    if(LBAwrite(de, D_ENTRY_BLOCKS, startBlock) == -1){
		printf("Write error.\n");
		free(de);
		return -1;
	}

	DirectoryEntry* de2 = (DirectoryEntry*) malloc(sizeof(DirectoryEntry));

	strcpy(de2->name, ".."); 
	de->size = ENTRY_MEM;
    de->location[1].size = startBlock;
    de->creation_date = t;
    de->last_modified = t;
	de->last_opened = t;
    de->type = 1;

	if(LBAwrite(de2, D_ENTRY_BLOCKS,  startBlock+1) == -1){
		printf("Write error.\n");
		free(de);
		free(de2);
		return -1;
	}
	

	return 0;
}


void exitFileSystem ()
	{
	printf ("System exiting\n");
	}



