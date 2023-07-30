#include "mfs.h"
#include "FreeSpace.h"

// Key directory functions
int fs_mkdir(const char *pathname, mode_t mode){
    printf("==> Make Directory <==\n");
    
    char pathToParse[256];
    strcpy(pathToParse, pathname);
    PathInfo * pathinfo = malloc(sizeof(PathInfo));
    parsePath(pathToParse, pathinfo);

    /*printf("isValidPath: %d\n", pathinfo->isValidPath);
    printf("isFileType: %d\n", pathinfo->isFileType);
    printf("index: %d\n", pathinfo->index); 
    printf("lastToken: %s\n", pathinfo->lastToken);
    printf("parent->size: %lu\n", pathinfo->parent->size);
    printf("parent->name: %s\n", pathinfo->parent->name);
    printf("parent->type: %d\n", pathinfo->parent->type);
    printf("parent->loc: %ld\n", pathinfo->parent->location[0].loc);
    printf("parent->count: %d\n", pathinfo->parent->location[0].count);
    */

    if(pathinfo->isValidPath == 1 && pathinfo->isFileType == -1){

    }
    //Time for Initializing Date information
    //GetFreeSpace returned to
    //  1. Set the date information in the Directory Entry of the Parent Directory
    //  2. Set the initial date information for the directory Entry array 
    time_t rawTime;
    int NewDirectoryFirstBlock = GetFreeSpace(D_ENTRY_BLOCKS);
    printf("NewDirectoryFirstBlock:%d\n", NewDirectoryFirstBlock);

    //Make a Directory Entry in the Parent Directory
    for(int i = 0; i < NUM_ENTRIES; i++){
        if(strcmp(pathinfo->parent[i].name, "") == 0 && pathinfo->parent[i].type == -1){
            pathinfo->parent[i].size = ENTRY_MEM;
            pathinfo->parent[i].location[0].loc = NewDirectoryFirstBlock;
            pathinfo->parent[i].location[0].count = D_ENTRY_BLOCKS;
            strcpy(pathinfo->parent[i].name,pathinfo->lastToken);
            pathinfo->parent[i].type = 1;
            pathinfo->parent[i].creation_date = time(&rawTime);	
            pathinfo->parent[i].last_modified = time(&rawTime);	
            pathinfo->parent[i].last_opened = time(&rawTime);

            printf("Free Directory Entry Found [%d] -> Set Data in Parent Directory\n", i); 	
            break;
        }
    }
    //Update the Parent Directory Block
    LBAwrite(pathinfo->parent, pathinfo->parent->location[0].count, pathinfo->parent->location[0].loc);
    //printf("LBAwritten\n");
    //Create a New Directory
    DirectoryEntry * directory = (DirectoryEntry*)malloc(NUM_ENTRIES * sizeof(DirectoryEntry));
    //printf("Directory mallocced\n");
    if (directory == NULL) {
        //Memory allocation failed
        printf("Memory allocation failed!\n");
        return 0;
    }

    //Initialize an Empty Directory Array
    printf("Initializing New Directory\n");	 
    for(int i = 0; i < NUM_ENTRIES; i++){
        directory[i].size = 0;
        for(int j = 0; j < 10; j++){
            directory[i].location[j].loc = 0;
            directory[i].location[j].count = 0;
        }
        strcpy(directory[i].name,"");
        directory[i].type = -1;
        directory[i].creation_date = time(&rawTime);	
        directory[i].last_modified = time(&rawTime);	
        directory[i].last_opened = time(&rawTime);	
    }
    //printf("Empty Directory initialized\n");	 

    //Initialize DirectoryEntry [0] as "."
    directory[0].size = ENTRY_MEM;
    directory[0].location[0].loc = NewDirectoryFirstBlock; //Root Direcotry Starts at block 6
    directory[0].location[0].count = D_ENTRY_BLOCKS; //Root DIrectory spans for 33 Blocks
    strcpy(directory[0].name,".");
    directory[0].type = 1; // This is a directory
    
    directory[1] = directory[0]; //".." is set to the info of "."
    strcpy(directory[1].name,".."); // rename it to ".."
    directory[1].location[0].loc = pathinfo->parent[0].location[0].loc;

   	 
    //Check "."
    // printf("\".\".size=%ld\n", directory[0].size);
    // printf("\".\".location[0]=%ld\n", directory[0].location[0].loc);
    // printf("\".\".name=%s\n", directory[0].name);	
    // printf("\".\".type=%d\n", directory[0].type);	
    // printf("\".\".creation_date=%ld\n", directory[0].creation_date);

    //Check ".."
    // printf("\"..\".size=%ld\n", directory[1].size);
    // printf("\"..\".location[0]=%ld\n", directory[1].location[0].loc);
    // printf("\"..\".name=%s\n", directory[1].name);	
    // printf("\"..\".type=%d\n", directory[1].type);	
    // printf("\"..\".creation_date=%ld\n", directory[1].creation_date);

    //Write the root directory
    LBAwrite(directory, D_ENTRY_BLOCKS, NewDirectoryFirstBlock);
    free(pathinfo);
    free(directory);
    printf("Direcotory Successfully Created\n"); 

    //Store root directory and set currentworkingdirectory to root
    return 0;
    
}
int fs_rmdir(const char *pathname) {
    PathInfo *pathinfo = malloc(sizeof(PathInfo));
    char pathToParse[256];
    strcpy(pathToParse, pathname);
    
    if (parsePath(pathToParse, pathinfo) == -1) { 
        printf("Path is not valid");
        free(pathinfo);
        return -1;
    }

    int DirentryLocation = 0; 
    int NumberOFbitsToFlip = 0;
    for (int i = 0; i < NUM_ENTRIES; i++) {
        if (strcmp(pathinfo->parent[i].name, pathinfo->lastToken) == 0) {
            strcpy(pathinfo->parent[i].name, "");
            pathinfo->parent[i].type = -1;
            
            DirentryLocation = pathinfo->parent[i].location[0].loc; 
            NumberOFbitsToFlip = pathinfo->parent[i].location[0].count;
            pathinfo->parent[i].location[0].loc = 0;
            pathinfo->parent[i].location[0].count = 0;
            
            LBAwrite(&pathinfo->parent[i], NumberOFbitsToFlip, DirentryLocation);
            break; // Assuming each directory name is unique, we can stop the loop here.
        }
    }

    ReleaseSpace(DirentryLocation, NumberOFbitsToFlip);
    free(pathinfo);
    //printf("Should work, and bits should be released");
    return 0;
}

// Directory iteration functions
fdDir* fs_opendir(const char* pathname) {
    PathInfo* pathinfo = malloc(sizeof(PathInfo));
    char pathToParse[256];
    strcpy(pathToParse, pathname);

    if (parsePath(pathToParse, pathinfo) == -1) {
        printf("Path is not valid");
        free(pathinfo);
        return NULL;
    }

    // Assuming pathinfo->parent contains the list of directory entries
    fdDir* dir = malloc(sizeof(fdDir));
    dir->d_reclen = sizeof(fdDir);        // Set the length of the fdDir structure
    dir->dirEntryPosition = 0;            // Start at the beginning of the directory
    dir->directory = pathinfo->parent;    // Set the pointer to the loaded directory
    dir->di = NULL;                       // Initialize the fs_diriteminfo pointer (for read)

    // Additional initialization if required
    // ...

    // Print the opened directory
    printf("Opened directory: %s\n", pathname);

    free(pathinfo);
    return dir;
}


struct fs_diriteminfo* fs_readdir(fdDir* dirp) {
    // Check if the given directory pointer is valid
    if (dirp == NULL || dirp->directory == NULL) {
        printf("Invalid directory pointer or directory not opened.\n");
        return NULL;
    }

    // Get the current directory entry position
    int currentEntryPosition = dirp->dirEntryPosition;

    // Check if the current entry position is within the valid range
    if (currentEntryPosition < 0 || currentEntryPosition >= NUM_ENTRIES) {
        printf("Invalid directory entry position.\n");
        return NULL;
    }

    // Get the current directory entry
    DirectoryEntry currentEntry = dirp->directory[currentEntryPosition];

    // Check if the current entry is used (i.e., not empty)
    if (!isUsed(currentEntry)) {
        printf("No more directory entries to read.\n");
        return NULL;
    }

    // Update the directory entry position for the next call to fs_readdir
    dirp->dirEntryPosition++;

    // Allocate memory for the fs_diriteminfo structure
    struct fs_diriteminfo *di = malloc(sizeof(struct fs_diriteminfo));
    if (di == NULL) {
        printf("Memory allocation failed for fs_diriteminfo.\n");
        return NULL;
    }

    // Set the fields of the fs_diriteminfo structure based on the current directory entry
    di->d_reclen = sizeof(struct fs_diriteminfo);
    di->fileType = (currentEntry.type == 1) ? FT_DIRECTORY : FT_REGFILE;
    strncpy(di->d_name, currentEntry.name, sizeof(di->d_name) - 1);
    di->d_name[sizeof(di->d_name) - 1] = '\0'; // Ensure null-termination of the string

    // Return the fs_diriteminfo structure containing the directory entry information
    return di;
}

int fs_closedir(fdDir* dirp) {
    // Free any dynamically allocated memory associated with the fdDir structure
    // For example, if you dynamically allocated memory for directory entries or fs_diriteminfo, free them here
    // Make sure to free any memory before setting the pointers to NULL to avoid memory leaks

    // Free the directory entries (assuming they were dynamically allocated)
    if (dirp->directory != NULL) {
        free(dirp->directory);
        dirp->directory = NULL;
    }

    // Free the fs_diriteminfo structure (assuming it was dynamically allocated)
    if (dirp->di != NULL) {
        free(dirp->di);
        dirp->di = NULL;
    }

    // Optionally, perform other cleanup operations if necessary

    // Finally, free the fdDir structure itself
    free(dirp);

    return 0;
}


// Misc directory functions
char* fs_getcwd(char* pathname, size_t size) {
    if (cwd == NULL) {
        // Current working directory not set
        return NULL;
    }

    // Check if the provided buffer size is sufficient to hold the current working directory
    size_t cwdLength = strlen(cwd->name);
    if (cwdLength >= size) {
        // Buffer size is insufficient to hold the current working directory
        return NULL;
    }

    // Copy the current working directory to the provided buffer
    strncpy(pathname, cwd->name, size);

    return pathname;
}

int fs_setcwd(char* pathname) {
    PathInfo* pathinfo = malloc(sizeof(PathInfo));
    char pathToParse[256];
    strcpy(pathToParse, pathname);

    if (parsePath(pathToParse, pathinfo) == -1) {
        printf("Path is not valid");
        free(pathinfo);
        return -1;
    }
    
    DirectoryEntry * cwdPtr; 

     for (int i = 0; i < NUM_ENTRIES; i++) {
        if (strcmp(pathinfo->parent[i].name, pathinfo->lastToken) == 0) {
           
             cwdPtr = &pathinfo->parent[i];

            
            break; // Assuming each directory name is unique, we can stop the loop here.
        }
     }
   // Gets the location of the parent
	 
    cwd = cwdPtr; 
    
    // Update the current working directory to the specified pathname
    // ... (add implementation details here, set the global current working directory)
    

    printf("%s", cwd->name);

    free(pathinfo);

    return 0;
}

int fs_isFile(char * filename){
    return  fs_isDir(filename);

}

int fs_isDir(char * pathname){
    //Look if entry is -1, error
    //If index is valid, look at bit to see whether or not its a directory
    //return 1 if Directory
    //return 0 otherwise

    PathInfo * pathinfo = malloc(sizeof(PathInfo));
    parsePath(pathname, pathinfo);
    if(pathinfo->isFileType == 1){
        free(pathinfo);
        return 0;
    }
    free(pathinfo);
    return 1;
}

int fs_delete(char* filename){
    //Now I know where I am with parsepath
    //Heres my directoryentry
    //This directory exists
    //If it doesnt, error
    //Its a file
    //If its not a file, error (delete only works on files)
    //Go ahead and release all the blocks
}	

int fs_stat(const char *path, struct fs_stat *buf){

}

int FindEntryInDir(DirectoryEntry * parent, char * token){
    // Iterate through the parent directory to find the matching token
    for(int i=0; i < NUM_ENTRIES; i++){
        if(strcmp(parent[i].name, token) == 0){
            return i;
        }
    }
    return -1;
}




DirectoryEntry * loadDir(DirectoryEntry parent) {
    DirectoryEntry * directory = malloc(sizeof(DirectoryEntry));
    if (!directory) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    LBAread(directory, parent.location[0].count, parent.location[0].loc); //We were given a DE
    return directory;
}

bool isDir(DirectoryEntry directoryEntry) {
    if(directoryEntry.type ==  1)
        return true;
    return false;
}

bool isUsed(DirectoryEntry de){
    if(strcmp(de.name, "") == 0 && de.type == -1)
        return false;
    return true;
}

int parsePath(char* path, PathInfo* pathInfo) {
    DirectoryEntry* startDir;
    DirectoryEntry* parent;
    char* token1;
    char* token2;

    //INIT PATHINFO
    pathInfo->index = -1;
    pathInfo->isFileType = -1;
    pathInfo->isValidPath = 0;
    pathInfo->lastToken = "";
    pathInfo->parent = parent;

    // Determine where the user is starting from
    // root if /
    // cwd if no /
    // Ex:
    // /hello/sam/chris.txt => start from root
    // hello/sam/christ.txt => start from current working directory
    if (path[0] == '/'){
        startDir == root;
    }
    else
        startDir = cwd;
    parent = startDir;

    // Read the first string
    token1 = strtok(path, "/");
    // Token 1 being NULL could mean "" or "/" -> Error
    if(token1 == NULL){
        printf("Error\n");
        return -1;
    }

    do{
        // Read the second string
        token2 = strtok(NULL, "/");

        // Get what entry Token1 is in the directory of parent
        // -1 if not found, 0-50 found
        int x = FindEntryInDir(parent, token1);

        // We have reached the end of the string
        // Token1 is the last entry
        if(token2 == NULL){
            pathInfo->isValidPath = 1;
            pathInfo->parent = parent;
            pathInfo->index = x;
            if (pathInfo->index == -1)
                pathInfo->isFileType = -1;
            else
                pathInfo->isFileType = (isDir(parent[x])) ? 0 : 1;
            pathInfo-> lastToken = token1;
            /*printf("=>ParsePathOutput<=\n");
            printf("pathInfo->parent[0],location[0].loc:%ld\n", pathInfo->parent[0].location[0].loc);
            printf("pathInfo->index:%d\n", pathInfo->index);
            printf("pathInfo->isFileType:%d\n", pathInfo->isFileType);
            printf("pathInfo->isValidPath:%d\n", pathInfo->isValidPath);
            printf("pathInfo-> lastToken:%s\n", pathInfo-> lastToken);
            */
            return 0; //Success
            //We want this case
        }

        // If no EntryIndex with the name of token1 exists in parent directory
        if (x == -1){
            printf("Error\n");
            return -1;
        }

        // Check if the Entry in the Parent Directory is a Directory
        if(!isDir(parent[x])){
            printf("Error\n");
            return -1;
        }

        // Load in the Directory using the Directory Entry at index given
        DirectoryEntry* temp = loadDir(parent[x]);

        if(parent != startDir)
            free(parent);

        // make the parent directory point to the next directory
        parent = temp;

        // move the two tokens forward
        // push token2 to token1
        token1 = token2;
    }while(token2 != NULL);
}