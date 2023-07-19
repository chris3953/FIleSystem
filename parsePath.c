#include <mfs.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include "parsePath.h"

void parsePath(DirectoryEntry * cwd, DirectoryEntry* root, char* pathToParse, parsedInfo * info ){
    char * prev = NULL; 
    char * curr = NULL; 
    DirectoryEntry * currentDir;					
    
    char * token = strtok(pathToParse, "/");	
    info->isFile = -1;							
    info->lastElementIndex = -1; 			
    
    if(pathToParse[0] == '/') 	
	    currentDir = root;
    else
	    currentDir = cwd;

    int dirIndex = 0;
    while(token != NULL){	
        prev = curr;						
	    curr = token;						
	    token = strtok(NULL, "/");	    
	
		if(token != NULL){ 					
	    	int index = 1; 
			while(currentDir->entries[++index] != NULL){
				if(!(strcmp(curr, currentDir->entries[index]->name))){
					currentDir = currentDir->entries[index];
					break;
				}

				if(currentDir->entries[index] == NULL){
					info->isPathValid = 0;
					return;
				}
			}
		} else if(token == NULL){
			info->parent = (currentDir == NULL) ? root : currentDir;		
			strcpy(info->newEntryName, curr);	
			info->isPathValid = 1; 
			int itr = 1;
			while(currentDir->entries[++itr] != NULL){ 
				if(!(strcmp(currentDir->entries[itr]->name, curr))){
					info->lastElementIndex = itr;
					info->isFile = currentDir->entries[itr]->isFile;
					return;
				}
			} 
			return;
		}
	}    
}  