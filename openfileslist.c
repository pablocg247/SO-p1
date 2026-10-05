#include "openfileslist.h"

static LISTASIMPLE L;

int OpenFilesAdd(int df, int mode, char name[]){
	OpenFile * newFile = malloc(sizeof(OpenFile));
	if (newFile == NULL){return -1;}
	newFile->df = df;
	newFile->mode = mode;
	strcpy(newFile->name,name);
	return AniadirElemento(L,newFile);
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

void PrintMode(int mode){
	
	}

void OpenFilesPrint(void* p){
	OpenFile* f = (OpenFile*)p;
	printf("descriptor: %d -> %s (", f->df, f->name);
	PrintMode(f->mode);
	printf(")\n");
}
void OpenFilesList(){
		ImprimirListaCompleta(L,0,OpenFilesPrint);
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
