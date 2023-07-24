/**************************************************************
* Class: CSC-415-02 Summer 2023
* Names: Andy Almeida, Masen Beacham, Christian Mcglothen, Kendrick Rivas
* ID's:  922170012, 918724721, 918406078, 922898506
* GitHub Name: chris3953
* Group Name: Beerman
* Project: Basic File System
*
* File: b_io.c
*
* Description: Basic File System - Key File I/O Operations
*
**************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>			// for malloc
#include <string.h>			// for memcpy
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "b_io.h"
#include "fsInit.h"
#include "mfs.h"
#include "FreeSpace.h"
#include "parsePath.h"
#include "fsLow.h"


#define MAXFCBS 20
#define B_CHUNK_SIZE 512

typedef struct b_fcb
	{
	DirectoryEntry * fi;
	
	int currLocation;
	int blockRemainder;
	int sizeRemainder;
	int bytesRead;

	/** TODO add all the information you need in the file control block **/

	// holds how many valid bytes are in the buffer
	int buflen;		

	// holds the open file buffer
	char * buf;	

	// holds the current position in the buffer
	int index;		
	} b_fcb;
	
b_fcb fcbArray[MAXFCBS];

// indicates it hasn't been initialized
int startup = 0;	

// method to initialize our file system
void b_init ()
	{
	// init fcbArray to all free
	for (int i = 0; i < MAXFCBS; i++)
		{
		// indicates a free fcbArray
		fcbArray[i].buf = NULL; 
		}
		
	startup = 1;
	}

// getting a free FCB element
b_io_fd b_getFCB ()
	{
	for (int i = 0; i < MAXFCBS; i++)
		{
		if (fcbArray[i].buf == NULL)
			{
			return i;	
			}
		}
	return (-1); 
	}
	
// interface to open a buffered file
b_io_fd b_open (char * filename, int flags)
	{
	b_io_fd returnFd;

	//*** TODO ***:  Modify to save or set any information needed
	
	printf("\n---Buffer open---\n");
	b_io_fd returnFd;
	DirectoryEntry * fi;
	char * buf;
	
	char pathToParse[NAME_LIMIT];
	strcpy(pathToParse, filename);
	PathInfo* info = malloc(sizeof(PathInfo));
	parsePath(cwd, root, pathToParse, info);

	if (info->lastElementIndex <= 0 | flags <= 0){
		printf("Error: Parse path failed.");
			return (-1);
	}

	 // initialize our system	
	if (startup == 0) b_init(); 
	buf = malloc(B_CHUNK_SIZE);
	
	if(buf == NULL){
		printf("Error: Malloc fail.");
			return (-1);
	}
	// get our own file descriptor
	
	returnFd = b_getFCB();				
										
	fcbArray[returnFd].buf = buf;
	fcbArray[returnFd].fi = fi;
	fcbArray[returnFd].buflen = 0;
	fcbArray[returnFd].index = 0;
	fcbArray[returnFd].currLocation = 0;
	fcbArray[returnFd].blockRemainder = (fi->size + (B_CHUNK_SIZE -1)) / B_CHUNK_SIZE;
	
	return (returnFd);		

	}

// interface to seek function	
int b_seek (b_io_fd fd, off_t offset, int whence)
	{
	// initialize our system
	if (startup == 0) b_init();  

	// fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		// invalid file descriptor
		return (-1); 					
		}
		
		
	// check if whence is EOF
	if(whence==SEEK_END){
		fcbArray[fd].index = fcbArray[fd].buflen - 1;

		// if SEEK_END, return length of buffer
		return fcbArray[fd].buflen;
	}
	// change index of given fd to offset
	fcbArray[fd].index = offset;	
		
	return fcbArray[fd].index;
	}

// interface to write function	
int b_write (b_io_fd fd, char * buffer, int count)
	{
	// initialize our system
	if (startup == 0) b_init();  

	// check fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		// invalid file descriptor
			return (-1); 					
		}
		
	char* blockBuf = malloc(B_CHUNK_SIZE);
	int ctr = 0, buffctr = 0;
	while( ctr < count ){

		if(buffctr == B_CHUNK_SIZE){
			LBAwrite(blockBuf, 1, fd);
			buffctr = 0;
		} else {
			blockBuf[buffctr++] = buffer[ctr++];
		}	
	}
	// resetting ctr for the next string 
	ctr = 0;

	//set remainder of buffer to null terminator
	while(buffctr != B_CHUNK_SIZE)
		blockBuf[buffctr++] = '\0';
		LBAwrite(blockBuf, 1, fd);
		return (0); 
	}

// Filling the callers request is broken into three parts
// Part 1 is what can be filled from the current buffer, which may or may not be enough
// Part 2 is after using what was left in our buffer there is still 1 or more block
//        size chunks needed to fill the callers request.  This represents the number of
//        bytes in multiples of the blocksize.
// Part 3 is a value less than blocksize which is what remains to copy to the callers buffer
//        after fulfilling part 1 and part 2.  This would always be filled from a refill 
//        of our buffer.
//  +-------------+------------------------------------------------+--------+
//  |             |                                                |        |
//  | filled from |  filled direct in multiples of the block size  | filled |
//  | existing    |                                                | from   |
//  | buffer      |                                                |refilled|
//  |             |                                                | buffer |
//  |             |                                                |        |
//  | Part1       |  Part 2                                        | Part3  |
//  +-------------+------------------------------------------------+--------+
int b_read (b_io_fd fd, char * buffer, int count)
	{
	// initialize our system
	if (startup == 0) b_init();  

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		// invalid file descriptor
			return (-1); 					
		}
		
	if(fcbArray[fd].fi == NULL){
		return -1;
	}

	// offset the count input
	int offset = 0;

	// total bytes transferred
	int bytesRead = 0;

	// processing data while count is > 0
	while(count != 0){
		// used to track position of block by giving offset
		int blockRemainderOffset = B_CHUNK_SIZE;

		// exit condition, after EOF is reached
		if(fcbArray[fd].sizeRemainder == 0){
			return 0;
		}

		// last proocessing when EOF is reached before exit condition
		if(fcbArray[fd].sizeRemainder <= 0 && fcbArray[fd].blockRemainder <= 0){
			
			// set to 0 for exit condition in subsequent calls
			fcbArray[fd].sizeRemainder = 0;
			break;
		}
		
		// empty block with data remaining in file to read, fill up bufferArray with block of data
		if( fcbArray[fd].blockRemainder == 0 ){
			// incrementing currLocation for next LBAread call
			LBAread(bufferArray[fd], 1, fcbArray[fd].currLocation++); 
			
			// tracking remaining size of file after LBAread, <=0 when empty
			fcbArray[fd].sizeRemainder-=B_CHUNK_SIZE; 		  
			fcbArray[fd].blockRemainder = (fcbArray[fd].sizeRemainder >= 0) ? B_CHUNK_SIZE 
			: fcbArray[fd].sizeRemainder + B_CHUNK_SIZE;
			
			// offset used to track current location in block for duration of function call
			blockRemainderOffset = fcbArray[fd].blockRemainder;
		}
		
		// full count can be transferred and exit while loop
		if(fcbArray[fd].blockRemainder >= count){ // condition (1)

			// transferring over count bytes due to condition (1)
            memcpy(buffer + offset, bufferArray[fd] + (blockRemainderOffset - 
			fcbArray[fd].blockRemainder), count);
			
			// reducing blockRemainder 
			fcbArray[fd].blockRemainder-=count;	
			
			// increasing bytesRead    
			bytesRead+=count;				
			
			// increasing accumulated bytesRead 
			fcbArray[fd].bytesRead+=count;

			// setting count to 0 since data is transferred
			count = 0;	

		} else {
			// remainder of block is transferred, and block will be refreshed 
			// with LBAread until condition is true
			memcpy(buffer + offset, bufferArray[fd] + (blockRemainderOffset - 
			fcbArray[fd].blockRemainder), fcbArray[fd].blockRemainder);
			
			// reducing count by blockRemainder to keep track of data transferred
			count-=fcbArray[fd].blockRemainder; 	
			
			// increasing offset for buffer to keep track of current location in buffer
			offset+=fcbArray[fd].blockRemainder;	
			
			// increasing bytesRead since due to the data being transferred to caller
			bytesRead+=fcbArray[fd].blockRemainder;

			// debug
			fcbArray[fd].bytesRead+=fcbArray[fd].blockRemainder; 

			// set to 0 to be in condition with LBAread due to count being >0
			fcbArray[fd].blockRemainder = 0;	
		}
	}
	// total transferred bytes
	return bytesRead;	
	}
	
// interface to Close the file	
int b_close (b_io_fd fd)
	{
		b_fcb* fcb = &fcbArray[fd];
		return 1;
	}
