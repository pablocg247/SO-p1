#include "openfileslist.h"

static LISTASIMPLE L;

int OpenFilesAdd(int df, int mode,const char name[]){
	OpenFile * newFile = malloc(sizeof(OpenFile));
	if (newFile == NULL){
		errno=ENOMEM;
		return -1;
	}

	newFile->df = df;
	newFile->mode = mode;
	strncpy(newFile->name,name,sizeof(newFile->name)-1);
	newFile->name[sizeof(newFile->name) - 1] = '\0';

	int result = AniadirElemento(L,newFile);
	if(result == -1){
		free(newFile);
		return -1;
	}
	return result;
}

int OpenFilesDel(int df){
	for(int i=0; i<MAXLISTASIMPLE && L[i] != NULL; i++){
		OpenFile* f = (OpenFile*)L[i];
		if(f->df == df){
			return BorrarElementoAtPos(L,i);
		}
	}
	return -1;
}

static void PrintMode(int mode){
	int accmode = mode & O_ACCMODE;

	if (accmode == O_RDONLY) {
		printf("O_RDONLY");
	} else if (accmode == O_WRONLY) {
		printf("O_WRONLY");
	} else if (accmode == O_RDWR) {
		printf("O_RDWR");
	}

	if (mode & O_CREAT)  printf(" O_CREAT");
	if (mode & O_EXCL)   printf(" O_EXCL");
	if (mode & O_TRUNC)  printf(" O_TRUNC");
	if (mode & O_APPEND) printf(" O_APPEND");
}

void OpenFilesList(){
	for(int i = 0; i < MAXLISTASIMPLE && L[i] != NULL; i++){
		OpenFile* f = (OpenFile*)L[i];
		printf("descriptor: %d -> %s (", f->df, f->name);
		PrintMode(f->mode);
		printf(")\n");
	}
}

OpenFile* OpenFilesGet(int df){
	for (int i = 0; i < MAXLISTASIMPLE && L[i] != NULL; i++) {
        OpenFile *f = (OpenFile *)L[i];
        if (f->df == df) {
            return f;
        }
    }
    return NULL;
}

void OpenFilesClear(){
	BorrarLista(L);
}
