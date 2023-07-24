/**************************************************************
* Class: CSC-415-02 Summer 2023
* Names: Andy Almeida, Masen Beacham, Christian Mcglothen, Kendrick Rivas
* ID's:  922170012, 918724721, 918406078, 922898506
* GitHub Name: chris3953
* Group Name: Beerman
* Project: Basic File System
*
* File: mfs.c
*
* Description: Basic File System - Key File I/O Operations
*
**************************************************************/
#include "mfs.h"
#include "parsePath.c"
#include "FreeSpace.h"

int fs_mkdir(const char *pathname, mode_t mode){
	printf("\n---> Make Directory <---\n");

	char pathToParse[NAME_LIMIT];

	strcpy(pathToParse, pathname);

	PathInfo* info = malloc(sizeof(PathInfo));

	parsePath(cwd, root, pathToParse, info);

	if(info-> isValidPath == 0)
		return (-2);

	if(info-> indexLastElement >= 0)
		return (-2);

	printf("MKDIR parent: %s\n", info->parentDirEntry->name);

	DirectoryEntry* newDir = createDir(info->lastElementName, 0, info->parentDirEntry);

	printf("Created dir: %s\n", newDir->name);

	//TODO: free info
	writeDir(newDir);

	return 0;
}

int fs_rmdir(const char *pathname){
	// /foo/bar/this/that means:
	// from parsePath, given info struct which contains parent
	// parent->entries[lastElementIndex] = the child, in this case,
	// parent = this, and parent->entries[lastElementIndex] = that
	// goal: remove "that" -> need to write remove directory

	printf("\n---Remove Directory---\n");

	char pathToParse[NAME_LIMIT];

	strcpy(pathToParse, pathname);

	PathInfo* info = malloc(sizeof(PathInfo));

	parsePath(cwd, root, pathToParse, info);

	printf("IN RMDIR . . .\n lastElementindex: %d, isFile: %d\n", info->indexLastElement, 
	info->parentDirEntry->entries[info->indexLastElement]->type);	

	if(info->indexLastElement< 0)
		return  (-2) ;	// if last element doesn't exist, error

	if(info->parentDirEntry->entries[info->indexLastElement]->type)
		return  (-2) ;	// if last element is not a directory (0 = dir, 1 = file), error

	DirectoryEntry* dirToDelete = loadDir(info->parentDirEntry->entries[info->indexLastElement]);
	// check if dirToDelete->entries is empty (only . and ..)

	int itr = 1;

	while(dirToDelete->entries[++itr] != NULL) // if dir is not empty, return error
		return (-2);

	return deleteEntry(info->parentDirEntry, info->indexLastElement); // 0 = success

}

int fs_delete(char* filename){

 	printf("\n---Deletey---\n");

	 char pathToParse[NAME_LIMIT];

        strcpy(pathToParse, filename);

        PathInfo* info = malloc(sizeof(PathInfo));

        parsePath(cwd, root, pathToParse, info);

        if(info->indexLastElement < 0)
                return (-2);    // if last element doesn't exist, error

        if(!info->parentDirEntry->entries[info->indexLastElement]->type)
                return (-2);    // if last element is not a directory (0 = dir, 1 = file), error

	return deleteEntry(info->parentDirEntry, info->indexLastElement);	// 0 = success

}


// Directory iteration functions
fdDir * fs_opendir(const char *name){
	printf("\n---Open Directory---\n");

	char pathToParse[NAME_LIMIT];

	strcpy(pathToParse, name);

	PathInfo* info = malloc(sizeof(PathInfo));

	parsePath(cwd, root, pathToParse, info);
	
	printf("-OPENDIR- parent name: %s LASTELEMENTINDEX: %d\n", info->parentDirEntry->name, 
	info->indexLastElement);	

	// TODO: Add if is directory condition
	if(info->indexLastElement!= 0){
		fdDir* PathInfo = malloc(sizeof(fdDir));

		PathInfo->directory = loadDir(info->parentDirEntry->entries[info->indexLastElement]);
		PathInfo->d_reclen = info->parentDirEntry->size/sizeof(DirectoryEntry);
		PathInfo->directoryStartLocation = info->parentDirEntry->location;
		PathInfo->dirEntryPosition = info->indexLastElement;

		return PathInfo;
	} else {
		printf("Error opening Directory! in mfs.c\n");
		free(info);
		return (-1);
	}	
}

// closes the directory
int fs_closedir(fdDir *dirp){
	printf("\n---Close Directory---\n");

	int itr = -1;

	while(dirp->directory->entries[++itr] != NULL){
		dirp->directory->entries[itr] = NULL;
		free(dirp->directory->entries[itr]);
	}

	dirp->directory = NULL;
	free(dirp->directory);
	free(dirp);

	return 0;
}


// Misc directory functions
char * fs_getcwd(char *buf, size_t size){
	printf("\n---Get Curent Working Directory---\n");
	
	// Copies the current working directory into the buffer
	strcpy(buf, cwd->name);

	// Returns the now copied buffer.
	return buf;
}

int fs_setcwd(char *buf){
	printf("\n---Set Current Working Directory---\n");

	// Parse Path
	char pathToParse[NAME_LIMIT];

	strcpy(pathToParse, buf);

	PathInfo* info = malloc(sizeof(PathInfo));

	parsePath(cwd, root, pathToParse, info);

	printf("-SETCWD- lastElementIndex: %d\n", info->indexLastElement);

	// Checking to see if the last element exists and is an element. 
	if(info->indexLastElement > 0){
		// Gets the location of the parent
		DirectoryEntry * cwdPtr = loadDir(info->parentDirEntry->entries[info->indexLastElement]);

		// Allocates the cwd to the length of the buffer.
		cwd = cwdPtr;
		
		return 0;
	} else {
		// Error message
		printf("Error in setcwd.");
		return -1;
	}
}

int fs_isFile(char * path){
	printf("\n---Is File---\n");

	//call parsepath, return the isFile field
	char pathToParse[NAME_LIMIT];

	strcpy(pathToParse, path);

	PathInfo* info = malloc(sizeof(PathInfo));

	parsePath(cwd, root, pathToParse, info);

	return info->isFileType;

}  

int fs_isDir(char * path){
	printf("\n---Is Directory---\n");
	//call isFile, return opposite.
	return !fs_isFile(path);

}   // return 1 if directory, 0 otherwise
struct fs_diriteminfo *fs_readdir(fdDir *dirp){
	
	fs_diriteminfo* newInfo = malloc(sizeof(fs_diriteminfo));

	// malloc buffer for LBAread
	DirectoryEntry* dirBuf = malloc(sizeof(DirectoryEntry));

	// read the directory from the volume/disk
	LBAread(dirBuf, dirp->d_reclen, dirp->directoryStartLocation);

	// increment current dirEntry pos
	dirp->dirEntryPosition += 1;

	//  "newInfo->___" instead of "newInfo.___"
	newInfo->d_reclen = dirp->d_reclen;
	newInfo->fileType = dirBuf->entries[dirp->dirEntryPosition]->type;

	// newInfo->d_name
	strcpy(newInfo->d_name, dirBuf->entries[dirp->dirEntryPosition]->name);

	return newInfo;
}


// TODO: error checks
int fs_stat(const char *path, struct fs_stat *buf){
	printf("\n---Stat---\n");

	char pathToParse[NAME_LIMIT];

	strcpy(pathToParse, path);

	PathInfo* info = malloc(sizeof(PathInfo));

	parsePath(cwd, root, pathToParse, info);
	
	DirectoryEntry* toInsert = info->parentDirEntry->entries[info->indexLastElement];
	buf->st_size = toInsert->size;
	buf->st_blksize = VCBPtr->block_size;
	buf->st_blocks = D_ENTRY_BLOCKS; 
	buf->st_accesstime = toInsert->creation_date;
	buf->st_modtime = toInsert->last_modified;
	buf->st_createtime = toInsert->creation_date;

	return 0; 

}

DirectoryEntry * createDir(char* name, int isFile, DirectoryEntry* parent){
	int index = -1;

	// create a DirectoryEntry in the parent's entries
	for(int i = 2; i < NUM_ENTRIES; i++){
		if(parent->entries[i] == NULL){
			index = i;
			break;
		}
	}

	if(index == -1)
		return NULL;
	
	// allocating memory for the new directory to be added
	DirectoryEntry* currentDir = malloc(sizeof(DirectoryEntry));

	int freeSpaceBlock = GetFreeSpace(D_ENTRY_BLOCKS);

	time_t rawTime;

	strcpy(currentDir->name, name);
	currentDir->size = ENTRY_MEM;
	currentDir->location = freeSpaceBlock;
	currentDir->creation_date = time( &rawTime );
	currentDir->type = isFile;
	
	// allocating memory for the entries of new directory to be added
	currentDir->entries = malloc(sizeof(DirectoryEntry*) * NUM_ENTRIES);

	// free state
	for(int i = 0; i < NUM_ENTRIES; i++){
		currentDir->entries[i] = NULL;
	}
	// initializing . (self) and .. (parent) directories
	currentDir->entries[0] = malloc(sizeof(DirectoryEntry));
	strcpy(currentDir->entries[0]->name, ".");
	currentDir->entries[0]->size = currentDir->size;
	currentDir->entries[0]->location = freeSpaceBlock;
	currentDir->entries[0]->creation_date = currentDir->creation_date;
	currentDir->entries[0]->type = currentDir->type;
	
	currentDir->entries[1] = malloc(sizeof(DirectoryEntry));
	strcpy(currentDir->entries[1]->name, "..");
	currentDir->entries[1]->size = parent->size;
	currentDir->entries[1]->location = parent->location;
	currentDir->entries[1]->creation_date = parent->creation_date;
	currentDir->entries[1]->type= parent->type;
	currentDir->entries[1]->entries = parent->entries;

	// setting . to go back to self
	currentDir->entries[0]->entries = currentDir->entries;

	// setting the new directory to the index of parent 
	parent->entries[index] = currentDir;
	
	writeDir(currentDir);	
	// return DirectoryEntry memory location of the child 

	return currentDir;
}

// LBAxxx( buffer, lbaCount, lbaPosition) lbaCount -> block #, lbaPosition -> block location
int writeDir(DirectoryEntry* entry){ // LBAwrite, returns location 
	return LBAwrite(entry, D_ENTRY_BLOCKS, entry->location); 

}

//LBAread reads location data into the given buffer
DirectoryEntry* loadDir(DirectoryEntry* parent){
	DirectoryEntry* buffer = malloc(sizeof(DirectoryEntry));
	LBAread(buffer, D_ENTRY_BLOCKS, parent->location);	
	return buffer;
}

// TODO: Free Space allocator
int deleteEntry(DirectoryEntry* parent, int elementIndex){

	if(parent == NULL)		
		return (-2);

	if(elementIndex < 0)
		return (-2);

	parent->entries[elementIndex] = NULL;

	free(parent->entries[elementIndex]);

	return 0; 
}	

