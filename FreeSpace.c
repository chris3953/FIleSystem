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
* Description: intialize and calculate freespace in the bitmap 
*
**************************************************************/
#include "FreeSpace.h"
#include "mfs.h"

//Function to initialize the bitmap (set all bits to 0)
void init_bitmap(Bitmap* bitmap){
    for (int i = 0; i < BITMAP_SIZE / (sizeof(unsigned long long int) * 8); i++) {
        bitmap->data[i] = 0;
    }
}

//Function to set a specific bit to 1
void set_bit(Bitmap * bitmap, int bit_index){
    int element_index = bit_index / (sizeof(unsigned long long int) * 8);
    int offset = bit_index % (sizeof(unsigned long long int) * 8);
    bitmap->data[element_index] |= (1ULL << offset);
}

//Function to set a specific bit to 0
void clear_bit(Bitmap* bitmap, int bit_index){
    int element_index = bit_index / (sizeof(unsigned long long int) * 8);
    int offset = bit_index % (sizeof(unsigned long long int) * 8);
    bitmap->data[element_index] &= ~(1ULL << offset);
}

//Function to check if a specific bit is set (1)
bool test_bit(Bitmap* bitmap, int bit_index){
    int element_index = bit_index / (sizeof(unsigned long long int) * 8);
    int offset = bit_index % (sizeof(unsigned long long int) * 8);
    return (bitmap->data[element_index] & (1ULL << offset)) != 0;
}

/*  Initialize Free Space
    Uses a bitmap for free space management
    Initialize the bits in the bitmap to set all bits to [0]:Unused block
    The bitmap is 19531 bits long, which is the same length of the 
    number of blocks in the freespace
    The first 6 bits will be then set to [1]: 0 for the VCB and 1-5 for the FreeSpaceMap
    [VCB 0: 1][BitMap: 1][BitMap: 1][BitMap: 1][BitMap: 1][BitMap: 1][0][0][0][0]...
*/ 
int InitFreeSpace(){
    //Create a BitMap -> 19531 bits or 2442 bytes -> 5 blocks
    //Malloc 5 blocks
    init_bitmap(&FreeSpaceMap);
    //Bits 0-5 are set to 1: Blocks 0,1,2,3,4,5 are used
    //[Bierman -: -][VCB 0: 1][BitMap 1: 1][BitMap 2: 1][BitMap 3: 1][BitMap 4: 1][BitMap 5: 1][6:0][0][0][0]...
    int nextAvailableBlock = 0;
    for(int i = 0; i < 6; i++){
        set_bit(&FreeSpaceMap,i);
	nextAvailableBlock++;
    }

    //Write that to disk with LABwrite(theFreeSpaceMap, 5, 1) 
    LBAwrite(&FreeSpaceMap, 5, 1);
    //Return the starting block number of the freespace to the VCB init
    return nextAvailableBlock++; //VCB lies in block 0
}

int SearchForSpace() {
    // Read the FreeSpaceMap from disk into the temporary bitmap
    LBAread(&FreeSpaceMap, 5, 1);

    // Iterate through the bitmap to find the first 0 bit (unused block)
    for (int i = 6; i < BITMAP_SIZE; i++) {
        if (!test_bit(&FreeSpaceMap, i)) {
            // Found an unused block, return its index
            return i;
        }
    }

    // If no 0 bit (unused block) is found, return -1 to indicate full storage
    return -1;
}



int GetFreeSpace(int NumberOfBlocks) {
    // Read the FreeSpaceMap from disk into the temporary bitmap
    LBAread(&FreeSpaceMap, 5, 1);

    // Initialize variables to keep track of the starting and ending index of the contiguous free space
    int startIdx = -1;
    int consecutiveBlocks = 0;

    // Iterate through the bitmap to find the required number of consecutive free bits
    for (int i = 0; i < BITMAP_SIZE; i++) {
        //printf("TestBit:[%d]=>%d\n",i, test_bit(&FreeSpaceMap, i));
        if (!test_bit(&FreeSpaceMap, i)) {
            //printf("Bit that is 0:%d\n", i);
            // If the bit is 0 (unused block), increase the count of consecutive free blocks
            if (consecutiveBlocks == 0) {
                // If this is the first free block in a potential contiguous sequence, record its index
                startIdx = i;
                //printf("startIdx:%d\n", startIdx);
            }
            consecutiveBlocks++;
            //printf("ConsecutiveBlocks++:%d\n", consecutiveBlocks);
            // Check if we have found the required number of consecutive free blocks
            if (consecutiveBlocks == NumberOfBlocks) {
                //printf("ConsecutiveBlocks == NumberOfBlocks\n");
                // Found the required number of consecutive free blocks, return the starting index
                for(int j = startIdx; j < startIdx + NumberOfBlocks; j++){
                    set_bit(&FreeSpaceMap, j);
                }
                //printf("Bitmap After Change\n");
                // for(int k = 0;k < 100; k++)
                //     printf("TestBit:[%d]=>%d\n",k, test_bit(&FreeSpaceMap, k));
                LBAwrite(&FreeSpaceMap, 5, 1);
                return startIdx;
            }
        } else {
            // If the bit is 1 (used block), reset the count of consecutive free blocks
            consecutiveBlocks = 0;
        }
    }

    // If no contiguous block of the required size is found, return -1 to indicate not enough free space
    return -1;
}


int ReleaseSpace(int Location, int NumberOfBlocks) {
    // Read the FreeSpaceMap from disk into the temporary bitmap
    LBAread(&FreeSpaceMap, 5, 1);

    // Release the specified number of blocks starting from the given location
    for (int i = Location; i < Location + NumberOfBlocks; i++) {
        // Clear the bit (set to 0) at the current index (release the block)
        clear_bit(&FreeSpaceMap, i);
    }

    // Write the updated FreeSpaceMap back to disk
    LBAwrite(&FreeSpaceMap, 5, 1);

    // Return 0 to indicate successful release of space
    return 0;
}
