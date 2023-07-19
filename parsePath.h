#include <stdlib.h> 

typedef struct {
    int isValidPath;               // 1 for valid / 0 for invalid
    int isFileType;                // 1 for file / 0 for directory / -1 if invalid 
    int indexLastElement;          // holds the index of the last element in the path
    char lastElementName[NAME_LIMIT]; // used to hold the name of the last element, optional
    directoryEntry* parentDirEntry;    // directoryEntry of the parent directory
    // add more fields here
} parsePath;

// Function to parse path
// Takes in the current working directory, root directory, the path to be parsed, and a reference to the PathInfo structure
void parsePath(DirectoryEntry * cwd, DirectoryEntry* root, char* pathToParse, PathInfo * pathInfo );