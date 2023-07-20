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
	strcpy(prev, curr);						// This logic will make it so that prev, curr, and token have the
	strcpy(curr, token);						// previous, current, and next entries.
	token = strtok(NULL, "/");	    
	    // if token is NOT Null -> case 1
	    // for each curr word, search in the myCwd's directoryEntries to see if it exists. 
	    // If it does exist, change myCwd = myCwd->entries[index]. If it doesn't, error
	    // if token IS Null -> case 2
	    // check if curr exists 
	    // if curr exists, find the index of curr from the parent or use .. ? 
	    // if curr doesn't exist, return the parent memory location
		if(token != NULL){ 						// case 1
	    	int index = 1; 
			// Looping through CWD entries to see if any match "curr".
			// If any match, set CWD to that entry and continue 
			while(myCwd->entries[++index] != NULL){
				if(!(strcmp(curr, myCwd->entries[index]->name))){
					myCwd = myCwd->entries[index];
					break;
				}
				// Error condition, no entries match the current token, therefore invalid path
				if(myCwd->entries[index] == NULL){
					info->isValidPath = 0;
					return;
				}
			}
		}
		// Case 2
		else if(token == NULL){ // NOTE: MyCwd == prev or the parent of "curr"
			if(myCwd == NULL)	// if prev is null, that means curr did not move, assuming parent is the current directory
				info->parentDirEntry = root;
			else
				info->parentDirEntry = myCwd;			
			strcpy(info->lastElementName, curr);	// Setting newEntryName to current, possibly not necessary to do

			info->isValidPath = 1; // at this process point, path is valid
			// next step: loop thru cwd->entries and see if curr exists
			// if it does, get index, filetype(?) and return
			// if it doesn't, lastElementIndex stays -1 to denote file not existing 
			
			int itr = 1; // 0 = ., 1 = .., in loop will be entries[2]
			while(myCwd->entries[++itr] != NULL){ // Finds last entry in parent entries
				if(!(strcmp(myCwd->entries[itr]->name, curr))){
					info->indexLastElement = itr;
					info->isFileType = myCwd->entries[itr]->type;
					return;
				}
			} 
			// at end of while, means curr was not found in myCwd->entries
			// This means that the path exists, but the file/directory does not. 
			return;
		}
	}
		    
    
}