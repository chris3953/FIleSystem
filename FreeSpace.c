/**************************************************************
* Class: CSC-415-02 Summer 2023
* Names: Andy Almeida, Masen Beacham, Christian Mcglothen, Kendrick Rivas
* ID's:  922170012, 918724721, 918406078, 922898506
* GitHub Name: chris3953
* Group Name: Beerman
* Project: Basic File System
*
* File: Freespace.c
*
* Description: Header for the main driver of the file system assignment,
*  where we will start and initialize our system.
*
**************************************************************/

#include <stdio.h>
#include <stdint.h>
#include "mfs.h"
#include "fsLow.h"
#include "FreeSpace.h"


#define FreeSpaceTotal 2560 
#define FreeSpaceLimit 8 * 2560

int LocationOfFreeSpace = 0; 
int SpaceNumber = 0; 
int spaceValue; 
unsigned char FreeSpace[FreeSpaceTotal]; 

int InitFreeSpace() { 

  for(int i = 0; i < FreeSpaceTotal; i++) { 
     FreeSpace[i] = 0; 
  }
  
  int RequestedSpace = GetFreeSpace(6); 
  RequestedSpace = RequestedSpace + 6; 

  LBAwrite(FreeSpace,5,1); 
  return RequestedSpace; 
}


int GetFreeSpace(int NumberOfBlocks){
	int byteNum = 0;
	int bitNum = SearchForSpace(); // returns number from 0 - 2560*8

	if(bitNum == -1) { 
       return -1; // error, no more space
	   spaceValue = bitNum;
    }
		
	for(int i = 1; i<= NumberOfBlocks; i++){
		byteNum = bitNum/8;
		FreeSpace[byteNum] = FreeSpace[byteNum] | ( 1 << (8- ((bitNum % 8) + 1)));
		bitNum++;
	}

	return spaceValue;
}

int SearchForSpace() { 

    int LocationOfBit = 0; 
    for (int byteNum = 0; byteNum < FreeSpaceTotal; byteNum++) {
        for (int bitNum = 0; bitNum < 8; bitNum++) {
            if (!(FreeSpace[byteNum] & (1 << (8 - (bitNum%8 + 1))))) {
                return LocationOfBit;
            }
            LocationOfBit++;
        }
    }
    
    return -1;
}

int ReleaseSpace(int Location , int NumberOfBlocks) { 

    int byteNum = 0;
	int bitNum = Location;
	for(int blocksToDel = 1; blocksToDel <= NumberOfBlocks; blocksToDel++){
		byteNum = bitNum/8;
		FreeSpace[byteNum] = FreeSpace[byteNum] & (~(1 << (8 - ((bitNum % 8) + 1))));
		bitNum++;
	}
}