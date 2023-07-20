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
    char * prev = NULL; 
    char * curr = NULL; 
    DirectoryEntry * currentDir;					
    
    char * token = strtok(pathToParse, "/");	
    info->isFileType = -1;							
    info->indexLastElement = -1; 			
    
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
					info->isValidPath = 0;
					return;
				}
			}
		} else if(token == NULL){
			info->parentDirEntry = (currentDir == NULL) ? root : currentDir;		
			strcpy(info->lastElementName, curr);	
			info->isValidPath = 1; 
			int itr = 1;
			while(currentDir->entries[++itr] != NULL){ 
				if(!(strcmp(currentDir->entries[itr]->name, curr))){
					info->indexLastElement = itr;
					info->isFileType = currentDir->entries[itr]->type;
					return;
				}
			} 
			return;
		}
	}    
}  