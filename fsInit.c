/**************************************************************
* Class: CSC-415-02 Summer 2023
* Names: Andy Almeida, Masen Beacham, Christian Mcglothen, Kendrick Rivas
  ID's:  922170012, 918724721, 918406078, 922898506
* GitHub Name: pie240
* Group Name: Beerman
* Project: Basic File System
*
* File: b_io.h
*
* Description: Intializes the file system with . and .. 
*  
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
//#include "ParsePath.h"

int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize)
	{
	printf ("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);
	/* TODO: Add any code you need to initialize your file system. */
	VCBPtr = malloc(blockSize);
	LBAread(VCBPtr, 1, 0); //Read block 0
	//Check if the signature matches
	if(VCBPtr->signature == SIGNATURE){
		printf("Signature in first Block Detected\n");
		printf("Space already initialized\n");
		int allocsize =(( NUM_ENTRIES * sizeof(DirectoryEntry)) + blockSize-1)/blockSize;
		allocsize = allocsize * blockSize;
		root = (struct DirectoryEntry*)malloc(allocsize);
		LBAread(root, D_ENTRY_BLOCKS, VCBPtr->root);
		printf("VCBPtr->root:%d\n", VCBPtr->root);
		printf("root called:\n");
		printf("root[0].name:%s\n", root[0].name);
		cwd = root;
		CurrentPath = "/";
		return 0;
	}

	// Initialize VCB info if SIGNATURE is not detected
	// SIGNATURE doesn't match = not initialized
	VCBPtr->signature= SIGNATURE;
	VCBPtr->totalBlocks = numberOfBlocks;
	VCBPtr->block_size = blockSize;
	VCBPtr->free_space_start_block = InitFreeSpace();
	VCBPtr->root = InitRootDirectory();
	LBAwrite(VCBPtr, 1, 0);
	return 0;
	}
	
int InitRootDirectory(){
	printf("Initializing Root Directory\n");
	//DirectoryEntry is 168 Bytes
	//We want 50 DirectoryEntries
	//Size of DirectoryEntries * Number of Entries = Directory Size
	//328 bytes * 50 Entries = 16400 Bytes Required for a Directory
	//Directory Size / BlockSize = Blocks Required for Directory
	//16400 Bytes / 512 Bytes per block = 33 (32.0312 rounded-up) Blocks Required for Directory
	//512 Bytes * 33 Blocks = 16896 Bytes
	// You can fit 51 Directory Entries in 33 Blocks
	// 51 Directory Entries * 328 bytes = 16728 bytes
	int allocsize =(( NUM_ENTRIES * sizeof(DirectoryEntry)) + MINBLOCKSIZE-1)/MINBLOCKSIZE;
	allocsize = allocsize * MINBLOCKSIZE;
	DirectoryEntry * directory = (struct DirectoryEntry*)malloc(allocsize);
	if (directory == NULL) {
		//Memory allocation failed
		printf("Memory allocation failed!");
		return 0;
	}

	time_t rawTime;
	for(int i = 0; i < NUM_ENTRIES; i++){
		directory[i].size = 0;
		for(int j = 0; j < 10; j++){
			directory[i].location[j].loc = 0;
			directory[i].location[j].count = 0;
		}
		strcpy(directory[i].name,"");
		directory[i].type = -1;
		directory[i].creation_date = time(&rawTime);	
		directory[i].last_modified = time(&rawTime);	
		directory[i].last_opened = time(&rawTime);	
	}	

	int RootDirectoryFirstBlock = GetFreeSpace(D_ENTRY_BLOCKS); 
	//printf("NextAvailableFreeSpace:%d\n", RootDirectoryFirstBlock);

	//Initialize DirectoryEntry [0] as "."
	directory[0].size = ENTRY_MEM;
	directory[0].location[0].loc = RootDirectoryFirstBlock; //Root Direcotry Starts at block 6
	directory[0].location[0].count = D_ENTRY_BLOCKS; //Root DIrectory spans for 33 Blocks
	strcpy(directory[0].name,".");
	directory[0].type = 1; // This is a directory
	
	directory[1] = directory[0]; //".." is set to the info of "."
	strcpy(directory[1].name,".."); // rename it to ".."

	//Write the root directory
	LBAwrite(directory, D_ENTRY_BLOCKS, RootDirectoryFirstBlock);

	//Store root directory and set currentworkingdirectory to root
	root = directory;
	cwd = root;
	CurrentPath = "/";	
	return RootDirectoryFirstBlock;
}
	
void exitFileSystem ()
	{
	printf ("System exiting\n");
	}



/*
	So, I think that directory entries are not only 2 bytes
	In my example in the Library, we imagined:
	DE FE BE DE DE DE DE DE    00 00 00 00 00 00 00 00
	00 00 00 00 00 00 00 00    00 00 00 00 00 00 00 00
	00 00 00 00 00 00 00 00    00 00 00 00 00 00 00 00
	00 00 00 00 00 00 00 00    00 00 00 00 00 00 00 00
	00 00 00 00 00 00 00 00    00 00 00 00 00 00 00 00
	00 00 00 00 00 00 00 00    00 00 00 00 00 00 00 00
	00 00 00 00 00 00 00 00    00 00 00 00 00 00 00 00
	00 00 00 00 00 00 00 00    00 00 00 00 00 00 00 00
	00 00 00 00 00 00 00 00    00 00 00 00 00 00 00 00

	I dont think thats the case. I think they are a bit bigger than that


*/
