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

#define SIGNATURE 0xBEEFED

VCB* vcb = NULL;

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
	}
	return 0;
	}
	
	
void exitFileSystem ()
	{
	printf ("System exiting\n");
	}
