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
#include "parsePath.h"

#define DEBUG 1 // Allows debugging by skipping VCBPtr->sig == signature check

DirectoryEntry * root;
DirectoryEntry * cwd;
void initRootDirectory(VCB* VCBPtr);




int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize)
	{
	printf ("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);
	/* TODO: Add any code you need to initialize your file system. */

	VCBPtr = malloc(blockSize);	// malloc block of memory
	LBAread(VCBPtr, 1, 0);		// grab VCBPtr to check if initialized correctly	
	
#if !DEBUG
	if((VCBPtr->sig == SIGNATURE))	// if signature is correct, return 0
		return 0;	
#endif
	// initializing Volume control block info if signature doesnt match
	// signature doesn't match = not initialized
	VCBPtr->signature= SIGNATURE;
	VCBPtr->totalBlocks = numberOfBlocks;
	VCBPtr->block_size = blockSize;
	VCBPtr->free_space_start_block = InitFreeSpace(); 	
	initRootDirectory(VCBPtr);	// init function sets the rootBlockNumber
	LBAwrite(VCBPtr, 1, 0);		// write data to hard disk / memory

	
		
	return 0; 			// return value unsure on 0
	}

void initRootDirectory(VCB* VCBPtr){
	// Deciding on 50 Directory Entries for now (based on MS1 steps)
	// DE Struct size is 40
	// Math works out to 4 blocks, 51 Directory Entries
	// 50 * 40 = 2000, 512 / 2000 = 3.9 -> approx. 4
	// 4 * 512 = 2048
	// 51*40 = 2040 -> 51 DEs

	
	// Asking for starting block of allocated space from freespace.c
	int startingBlock = GetFreeSpace(D_ENTRY_BLOCKS);
	time_t rawTime;					// rawtime for time part of entry
	root = malloc(sizeof(DirectoryEntry));	// malloc space for entries
	root->entries = malloc(sizeof(DirectoryEntry*) * NUM_ENTRIES);
	for(int i = 0; i < NUM_ENTRIES ; i++){
                root->entries[i] = NULL; // Null is free state  
        }
	
	strcpy(root->name, "/");
	root->size = ENTRY_MEM;
	root->location = startingBlock;
	root->creation_date = time( &rawTime);
	root->type = 0;

	// Initializing member variables of the struct for "." and ".." -> root directories
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
	//directoryEntry* home = createDir("home", 0, root);
	//directoryEntry* var = createDir("var", 0, home);
	//directoryEntry* this = createDir("this", 0, var);
	//directoryEntry* that = createDir("that", 0, this);
	//parsePath(cwd, root, pathToParse, info);
	//printf("after parsePath: isFile: %d, isPathValid: %d, lastElementIndex: %d, parent name: %s\n", info->isFile, info->isPathValid, info->lastElementIndex, info->parent->name); 	
	LBAwrite(root, D_ENTRY_BLOCKS, startingBlock);
	VCBPtr->root = startingBlock; // setting rootBlockNum to beginning or root dir
}


	
void exitFileSystem (){
	printf ("System exiting\n");
	free(VCBPtr); 
	VCBPtr = NULL;
	for(int i = 0; i < NUM_ENTRIES; i++){
		if(root->entries[i] != NULL){
			free(root->entries[i]); // freeing allocated space
			root->entries[i] = NULL; // setting null -> known free state
		}

	}
	}

