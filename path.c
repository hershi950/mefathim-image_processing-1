#define __path_c


#include <direct.h>
#include <stdlib.h>
#include <string.h>

#include "path.h"
#include "str_util.h"



//#define MAX_PATH 256


#pragma warning( disable : 4996)

#define str_len (int)strlen


int pathCopyParts(char *folder, int zFolder, char *name, int zName, char *ext, int zExt, const char *path)
{
	const char *pname, *pext;
	pathBreak(&pname, &pext, path);
	if (folder)
		str_make(folder, zFolder, path, (int)(pname - path));
	if (name)
		str_make(name, zName, pname, (int)(pext - pname));
	if (ext)
		str_2_str(ext, zExt, pext);
	return 0;
}
int pathCopyFolder(char *folder, int zFolder, const char *path)
{
	return pathCopyParts(folder, zFolder, NULL, 0, NULL, 0, path);
}
int pathCopyName(char *name, int zName, const char *path)
{
	return pathCopyParts(NULL, 0, name, zName, NULL, 0, path);
}
int pathCopyNameExt(char *name, int zName, const char *path)
{
	char ext[100];
	int ret = pathCopyParts(NULL, 0, name, zName, ext, sizeof(ext), path);
	str_append(name, zName, ext, (char *)0);
	return ret;
}
int pathCopyExt(char *ext, int zExt, const char *path)
{
	return pathCopyParts(NULL, 0, NULL, 0, ext, zExt, path);
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
// Return the address of the dot starting the path extension or null if no extension is present
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
const char *pathExtension(const char *path)
{
	int len, len1; 
	if (!path)
		return (char *)0;
	len = len1 = str_len(path);

	while (--len > 0)
		if (path[len] == '.')
			break;
	if (len < 0)
		return (char *)0;
	if (len == len1 - 1)
		return (char *)0;
	return path + len;
}


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
// Compute the length of the folder part including the trailing slash/backslash, if present.
// See comments at header.
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
int pathFolderLen(const char pathName[])
{
	const char *name, *ext;
	pathBreak(&name, &ext, pathName);
	return (int)(name - pathName);
}


#if 0
static void _folder_create(const char path[], int lenPath);
void pathFolderCreate(const char *folder)
{
	_folder_create(folder, (int)strlen(folder));
}
#endif

#if 0
void pathCreate(const char *pathName)
{
	const char *name, *ext;
	pathBreak(&name, &ext, pathName);
	if (name != pathName)
		_folder_create(pathName, (int)(name - pathName));
}
#endif


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
// Check if path ends with the specified extension. Comparison is CaSe InSeNsItIvE. Returns true/false
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
int pathIsExtension(const char path[], const char *ext)
{
	if (!ext)
		return 0;
	if (*ext == '.')
		ext++;
	if (!(*ext))
		return 0;

	const char *container = pathExtension(path);
	if (container)
		return !stricmp(container + 1, ext);
	return 0;
}


// -------------------------------
// Break a path-name. See header
// -------------------------------
const char *pathBreak(const char **name, const char **ext, const char *pathName)
{
	const char *N = pathName, *E = NULL, *F = pathName;

	// find name and extension
	// -----------------------
	while (*pathName) {
		if (*pathName == ':' || *pathName == '/' || *pathName == '\\')
			N = pathName + 1;
		else if (*pathName == '.')
			E = pathName;
		pathName++;
	}


	if (N > E) // Extension must follow name 
		E = NULL;

	// What was not found points to the trailing zero byte of the path-name
	if (!E)
		E = pathName;
	if (N == F)
		F = pathName;

	if (name)
		*name = N;
	if (ext)
		*ext = E;
	return F;
}


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
//				Construct a path-name given a folder-path and a file-name. 
// See comments at header.
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
char *pathMake(char *path, int zPath, const char *folder, const char *file)
{
	int	lenFile, lenFolder,lenNetFile, lenNetFolder;
	int lenPath = 0;


	// Sanity checks: 
	// 1. There is at least a file or a folder (might be both)
	// 2. we have some place to return the path
	// -------------------------------------------------------
	if (!path)
		zPath = 0;
	lenNetFile   = file   ? str_net_len(file) : 0;
	lenNetFolder = folder ? str_net_len(folder) : 0;
	if (!lenNetFile && !lenNetFolder)
		return (char *)0;

	lenFile   = file   ? str_len(file)   : 0;
	lenFolder = folder ? str_len(folder) : 0;
	if (lenFile + lenFolder + 1 > zPath)
		return (char *)0;



	// Path starts with the folder, if it is present at all
	// ----------------------------------------------------
	if (lenNetFolder) {
		strcpy(path, folder);
		str_ltrim(str_trim(path));
		lenPath = lenNetFolder;
	}


	// Do we need to insert an extra folder separator? 
	// -----------------------------------------------
	if (lenNetFile && lenPath && path[lenPath - 1] != '/' && path[lenPath - 1] != '\\')
		path[lenPath++] = '/';


	// Append the file name to the path, if present
	// --------------------------------------------
	if (lenNetFile) {
		strcpy(path + lenPath, file);
		str_ltrim(str_trim(path + lenPath));
		lenPath += lenNetFile;
	}

	path[lenPath] = '\0';
	return path;
}

const char *pathFileName(const char *path)
{
	return path + pathFolderLen(path);
}



const char *pathCWD()
{
	static char cwd[PATH_MAX];
	return _getcwd(cwd, sizeof(cwd));
}





