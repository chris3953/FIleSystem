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
* Description: Header for Freespace.c in which create the functions below 
*
**************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define BITMAP_SIZE 19531 //19531 bits for 19531 blocks = 2442 bytes = 5 blocks

typedef struct {
    unsigned long long int data[BITMAP_SIZE / (sizeof(unsigned long long int) * 8)];
} Bitmap;

//Function to initialize the bitmap (set all bits to 0)
void init_bitmap(Bitmap* bitmap);

//Function to set a specific bit to 1
void set_bit(Bitmap * bitmap, int bit_index);

//Function to set a specific bit to 0
void clear_bit(Bitmap* bitmap, int bit_index);

//Function to check if a specific bit is set (1)
bool test_bit(Bitmap* bitmap, int bit_index);

Bitmap FreeSpaceMap;

/*  Initialize Free Space
    Uses a bitmap for free space management
    Initialize the bits in the bitmap to set all bits to [0]:Unused block
    The bitmap is 19531 bits long, which is the same length of the 
    number of blocks in the freespace
    The first 6 bits will be then set to [1]: 0 for the VCB and 1-5 for the FreeSpaceMap
    [VCB 0: 1][BitMap: 1][BitMap: 1][BitMap: 1][BitMap: 1][BitMap: 1][0][0][0][0]...
*/ 
int InitFreeSpace(); 
int SearchForSpace(); 
int GetFreeSpace(int NumberOfBlocks); 
int ReleaseSpace(int Location , int NumberOfBlocks);
