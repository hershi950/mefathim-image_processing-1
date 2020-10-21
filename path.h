#ifndef __path_h
#define __path_h

#ifndef __c_linkage_h
#include "c_linkage.h"
#endif

#ifndef  PATH_MAX
#define PATH_MAX 256
#endif


C_LINKAGE_BEGIN


// ================================================================================================
// Return path of current working dorectory or null in case of error
// ================================================================================================
const char *pathCWD();


// ================================================================================================
// Return the address of the dot starting the path extension null if no extension is present
// ================================================================================================
const char *pathExtension(const char *path);


// ================================================================================================
// Compute the length of the folder part including the trailing slash/backslash, if present.
// Some subtle issue arises:
// e.g. x:/y/z --> 5 since 'z' is assumed not to be a folder
//      x:/y/z/ --> 7 since 'z' is indeed a folder.
// However 'z' could have been a folder in the first example as well
// If no folder is present in the path, zero is returned
// ================================================================================================
int pathFolderLen(const char pathName[]);


void pathFolderCreate(const char *folder);


// ================================================================================================
// Check if path ends with the specified extension. Comparison is CaSe InSeNsItIvE. Returns true/false
// ================================================================================================
int pathIsExtension(const char path[], const char *ext);


const char *pathFileName(const char *path);

// ================================================================================================
//						Construct a path-name given a folder-path and a file-name. 
// Either folder-path or file-name maybe be null or empty strings, but not both.
// The path is constructed into 'path' buffer that is 'zPath' bytes in size.
// The function return the addrss of 'path' if everything is ok. If an error occurs
// a null pointer is returned. Eror occurs if 'path' is null or 'zPath' is to small to 
// accomodate the concatenation of the folder and file.
//
// Trailing and/or leading whites of the folder and/or file are removed from the constrcuted path
//  
// examples
// pathMake (..., ..., "a/b/c", "x.dat")  --> "/a/b/c/x.dat". Note the extra slash that was added
// pathMake (..., ..., "a/b/c/", "x.dat") --> "/a/b/c/x.dat"
// pathMake (..., ..., NULL, "x.dat")     --> "x.dat"
// pathMake (..., ..., "foo/bar", NULL)   --> "foo/bar". No extra '/' is added since there is no name
// pathMake (..., ..., "foo/bar/", NULL)  --> "foo/bar/"
// ================================================================================================
char *pathMake(char *path, int zPath, const char *folder, const char *file);


// ================================================================================================
// Return the addresses of: folder(s), name and extension.
// If name does not exist (e.g. path= /a/b/c/) name will point to the zero terminating byte of
// the path. Likewise if extension is not present.
// The length of the the folder part is name - path. The length of the name part is ext - name
// ================================================================================================
const char *pathBreak(const char **name, const char **ext, const char *path);

int pathCopyParts(char *folder, int zForlder, char *name, int zName, char *ext, int zExt, const char *path);
int pathCopyFolder(char *folder, int zForlder, const char *path);
int pathCopyName(char *name, int zName, const char *path);
int pathCopyNameExt(char *nameExt, int zNameExt, const char* path);
int pathCopyExt(char *ext, int zExt, const char *path);


C_LINKAGE_END

#endif // multipel inclusion
