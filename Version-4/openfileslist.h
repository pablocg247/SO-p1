#ifndef OPENFILESLIST_H
#define OPENFILESLIST_H

#include "listasimple.h"
#include <string.h>

#define MAXFILENAME 1024

struct OpenFile{
	int df;
	int mode;
	char name[MAXFILENAME]
	}

int OpenFilesAdd(OpenFile d);
int OpenFilesDel(OpenFile d);
void OpenFilesList();
OpenFile OpenFilesGet(char name[]):
void OpenFilesClear();


#endif

