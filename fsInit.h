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
* Description: Where we defined our structures, VCB, Directory Entry, Extents etc 
*  
*
**************************************************************/



#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "fsLow.h"
#include "mfs.h"
//#include "FreeSpace.h"

#define SIGNATURE 0xBEEFED
#define NUM_ENTRIES 51
#define DIR_ENTRY_SIZE 168 //DirectoryEntry is 168 Bytes
#define ENTRY_MEM NUM_ENTRIES * DIR_ENTRY_SIZE
#define D_ENTRY_BLOCKS 33 

#ifndef FSINIT_H
#define FSINIT_H

// Place your original content of fsInit.h here
//fsLOW.c is not getting detected/is not present...
//but we never got it???


typedef struct Extents { 
  unsigned long loc; 
  unsigned int count; 

} Extents; 

typedef struct DirectoryEntry { 
   unsigned long size; 
   Extents location[10]; 
   char name[128]; 
   int type; // 1 if entry is a directory, 0 if a file, -1 if entry is free
    
   time_t creation_date; // date the entry was created
   time_t last_modified; // date the entry was last modified
   time_t last_opened; // date the entry was last opened
   //Size of this structure 328 bytes
}DirectoryEntry; 

typedef struct VCB {
    unsigned long signature;
    int root; // the block the root directory starts on
    int totalBlocks; 
    int block_size; // size of a block in bytes
    int free_space_start_block; // block at which free space starts
    //Size of this structure: 24 bytes
} VCB;

VCB *VCBPtr; //A pointer to the Volume Control Block 

int InitRootDirectory();

#endif
