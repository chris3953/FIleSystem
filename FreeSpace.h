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
int InitFreeSpace(); 
int SearchForSpace(); 
int GetFreeSpace(int NumberOfBlocks); 
int ReleaseSpace(int Location , int NumberOfBlocks); 