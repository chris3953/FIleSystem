/**************************************************************
* Class: CSC-415-02 Summer 2023
* Names: Andy Almeida, Masen Beacham, Christian Mcglothen, Kendrick Rivas
  ID's:  922170012, 918724721, 918406078, 922898506
* GitHub Name: chris3953
* Group Name: Beerman
* Project: Basic File System
*
* File: fsInit.h
*
* Description: Header for the main driver of the file system assignment,
*  where we will start and initialize our system.
*
**************************************************************/
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "fsLow.h"

typedef struct Extents { 
  unsigned long size; 
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

}DirectoryEntry; 

typedef struct VCB {

    int root; // the block the root directory starts on
    int totalBlocks; 
    int block_size; // size of a block in bytes
    int free_space_start_block; // block at which free space starts
    unsigned long signature; // magic number used to tell if the volume is initialized

} VCB;
