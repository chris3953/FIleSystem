/**************************************************************
* Class: CSC-415-02 Summer 2023
* Names: Andy Almeida, Masen Beacham, Christian Mcglothen, Kendrick Rivas
* ID's:  922170012, 918724721, 918406078, 922898506
* GitHub Name: chris3953
* Group Name: Beerman
* Project: Basic File System
*
* File: fsInit.h
*
* Description: Header for the main driver of the file system assignment
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

#define SIGNATURE 0xBEEFED
#define NUM_ENTRIES 51
#define DIR_ENTRY_SIZE 40
#define ENTRY_MEM NUM_ENTRIES * DIR_ENTRY_SIZE
#define D_ENTRY_BLOCKS 4 


#ifndef FSINIT_H
#define FSINIT_H

// Place your original content of fsInit.h here


typedef struct Extents { 
  unsigned long size; 
  unsigned int count; 

} Extents; 

typedef struct DirectoryEntry { 
   unsigned long size; 
   int location; 
   char name[128]; 
   int type; // 1 if entry is a directory, 0 if a file, -1 if entry is free
    
   time_t creation_date; // date the entry was created
   time_t last_modified; // date the entry was last modified
   time_t last_opened; // date the entry was last opened
   struct DirectoryEntry ** entries;


}DirectoryEntry; 

typedef struct VCB {
    unsigned long signature;
    int root; // the block the root directory starts on
    int totalBlocks; 
    int block_size; // size of a block in bytes
    int free_space_start_block; // block at which free space starts
     // magic number used to tell if the volume is initialized

} VCB;



#endif
