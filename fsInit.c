/**************************************************************
* Class: CSC-415-02 Summer 2023
* Names: Andy Almeida, Masen Beacham, Christian Mcglothen, Kendrick Rivas
* ID's:  922170012, 918724721, 918406078, 922898506
* GitHub Name: chris3953
* Group Name: Beerman
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
#include "parsePath.h"

// allows debugging by skipping VCBPtr
//sig == signature check
#define DEBUG 1 

DirectoryEntry * cwd;
DirectoryEntry * root;

void initRootDirectory(VCB* VCBPtr);

int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize)
	{
	printf ("Initializing File System with %ld blocks with a block size of %ld\n", 
	numberOfBlocks, blockSize);

	/* TODO: Add any code you need to initialize your file system. */

	// malloc block of memory
	VCBPtr = malloc(blockSize);

	// grab VCBPtr to check if initialized the right way
	LBAread(VCBPtr, 1, 0);			
	
#if !DEBUG
	if((VCBPtr->sig == SIGNATURE))	// if signature is correct, return 0
		return 0;	
#endif
	// signature doesn't match = not initialized
	VCBPtr->signature= SIGNATURE;

	VCBPtr->totalBlocks = numberOfBlocks;

	VCBPtr->block_size = blockSize;

	VCBPtr->free_space_start_block = InitFreeSpace(); 	

	// init function sets the rootBlockNumber
	initRootDirectory(VCBPtr);	

	// write data to hard disk / memory
	LBAwrite(VCBPtr, 1, 0);		

	return 0; 
	}

void initRootDirectory(VCB* VCBPtr){

	// asking for starting block of allocated space from freespace.c
	int startingBlock = GetFreeSpace(D_ENTRY_BLOCKS);

	// rawtime for time part of entry
	time_t rawTime;					
	
	// malloc space for entries
	root = malloc(sizeof(DirectoryEntry));	
	root->entries = malloc(sizeof(DirectoryEntry*) * NUM_ENTRIES);

	for(int i = 0; i < NUM_ENTRIES ; i++){
                root->entries[i] = NULL; 
        }
	
	strcpy(root->name, "/");
	
	root->size = ENTRY_MEM;

	root->location = startingBlock;

	root->creation_date = time( &rawTime);

	root->type = 0;

	// initializing member variables of the struct for "." and ".." 
	root->entries[0] = malloc(sizeof(DirectoryEntry));

	strcpy(root->entries[0]->name, ".");	

	root->entries[0]->size = ENTRY_MEM;

	root->entries[0]->location = startingBlock;

	root->entries[0]->creation_date = time( &rawTime );

	root->entries[0]->type = 0;

	root->entries[0]->entries = root->entries;
	
	
	// printf("address: %llx\n", (unsigned long long int)entries[0]); 
	root->entries[1] = malloc(sizeof(DirectoryEntry));

	strcpy(root->entries[1]->name, "..");

	root->entries[1]->size = ENTRY_MEM;

	root->entries[1]->location = startingBlock;

	root->entries[1]->creation_date= time( &rawTime );

	root->entries[1]->type = 0;

	root->entries[1]->entries = root->entries;

	cwd = root;
	PathInfo * info = malloc(sizeof(PathInfo));

	char pathToParse[NAME_LIMIT] = "/home/var/this/that";
	
	LBAwrite(root, D_ENTRY_BLOCKS, startingBlock);

	VCBPtr->root = startingBlock; 
}


void exitFileSystem (){

	printf ("System exiting\n");

	free(VCBPtr); 

	VCBPtr = NULL;

	for(int i = 0; i < NUM_ENTRIES; i++){
		if(root->entries[i] != NULL){
			
			// freeing allocated space
			free(root->entries[i]);

			// setting null -> known free state
			root->entries[i] = NULL; 
		}
	}
	}

