/**************************************************************
* Class: CSC-415-02 Summer 2023
* Names: Andy Almeida, Masen Beacham, Christian Mcglothen, Kendrick Rivas
* ID's:  922170012, 918724721, 918406078, 922898506
* GitHub Name: chris3953
* Group Name: Beerman
* Project: Basic File System
*
* File: parsePath.c
*
* Description: Parse path function to parse the path of a given string
*
**************************************************************/
#include <mfs.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "fsInit.h"
#include "parsePath.h"

void parsePath(DirectoryEntry * cwd, DirectoryEntry* root, char* pathToParse, PathInfo * info ){

    char prev[NAME_LIMIT] = ""; 
    char curr[NAME_LIMIT] = "";
    DirectoryEntry * myCwd;						
    char * token = strtok(pathToParse, "/");	 
    
    info->isFileType = -1;							
    info->indexLastElement = -1; 				
    if(pathToParse[0] == '/') 					
	    myCwd = root;
    else
	    myCwd = cwd;
	
    int i = 0;
    while(token != NULL){	
	// this logic will make it so that prev, curr, and token have the
	// previous, current, and next entries.
	strcpy(prev, curr);						
	strcpy(curr, token);	

	token = strtok(NULL, "/");	    

		// if token is NOT Null -> case 1
	    // for each curr word, search in the myCwd's directoryEntries to see if it exists. 
		if(token != NULL){ 				
	    	int index = 1; 

			// looping through CWD entries to see if any match "curr".
			while(myCwd->entries[++index] != NULL){
				if(!(strcmp(curr, myCwd->entries[index]->name))){
					myCwd = myCwd->entries[index];
					break;
				}
				// no entries match the current token, therefore invalid path
				if(myCwd->entries[index] == NULL){
					info->isValidPath = 0;
					return;
				}
			}
		}
		// case 2
		else if(token == NULL){
			// if prev is null, that means curr did not move 
			if(myCwd == NULL)	
				info->parentDirEntry = root;
			else
				info->parentDirEntry = myCwd;			
			strcpy(info->lastElementName, curr);

			info->isValidPath = 1; 
			
			int itr = 1; 
			while(myCwd->entries[++itr] != NULL){ 
				// finds last entry in parent entries
				if(!(strcmp(myCwd->entries[itr]->name, curr))){
					info->indexLastElement = itr;
					info->isFileType = myCwd->entries[itr]->type;
					return;
				}
			} 
			return;
		}
	}
		    
    
}