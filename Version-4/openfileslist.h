#ifndef OPENFILESLIST_H
#define OPENFILESLIST_H

#include "listasimple.h"
#include <string.h>

#define MAXFILENAME 1024

typedef struct OpenFile{
	int df;
	int mode;
	char name[MAXFILENAME];
	}OpenFile;

int OpenFilesAdd(int df, int mode, char name[]);
int OpenFilesDel(int df);
void OpenFilesPrint(void *p);
void OpenFilesList();
OpenFile* OpenFilesGet(int df);
void OpenFilesClear();


#endif

