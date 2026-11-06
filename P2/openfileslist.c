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
	errno = ENOENT;
	return -1;
}

int OpenFilesInit(){
    long limite = sysconf(_SC_OPEN_MAX);

    if (limite == -1) {
        printf("No se pudo obtener el limite de descriptores\n");
        return -1;
    }

    for (long n = 0; n < limite && n <= INT_MAX; n++) {
        int df = (int)n;
        int mode = fcntl(df, F_GETFL);

        if (mode == -1) {
            if (errno == EBADF)
                continue;

            perror("Imposible consultar descriptor");
            return -1;
        }

        if (OpenFilesGet(df) != NULL)
            continue;

        char nombre[MAXFILENAME];

        if (df == STDIN_FILENO) {
            snprintf(nombre, sizeof(nombre), "entrada estandar");
        }
        else if (df == STDOUT_FILENO) {
            snprintf(nombre, sizeof(nombre), "salida estandar");
        }
        else if (df == STDERR_FILENO) {
            snprintf(nombre, sizeof(nombre), "error estandar");
        }
        else {
            char ruta[64];

            snprintf(ruta, sizeof(ruta), "/proc/self/fd/%d", df);

            ssize_t leidos = readlink(ruta, nombre, sizeof(nombre) - 1);

            if (leidos == -1) {
                snprintf(nombre, sizeof(nombre),
                         "descriptor heredado %d", df);
            }
            else {
                nombre[leidos] = '\0';
            }
        }

        if (OpenFilesAdd(df, mode, nombre) == -1) {
            perror("Imposible registrar descriptor heredado");
            return -1;
        }
    }

    return 0;
}		


static void PrintMode(int mode){
    if (mode & O_CREAT)  printf("O_CREAT ");
    if (mode & O_EXCL)   printf("O_EXCL ");
    if (mode & O_TRUNC)  printf("O_TRUNC ");
    if (mode & O_APPEND) printf("O_APPEND ");

    switch (mode & O_ACCMODE) {
        case O_WRONLY:
            printf("O_WRONLY ");
            break;

        case O_RDWR:
            printf("O_RDWR ");
            break;

        case O_RDONLY:
            /* La referencia no imprime O_RDONLY */
            break;
    }
}

void OpenFilesList(){
	int maximo = 19;

    for (int i = 0; i < MAXLISTASIMPLE && L[i] != NULL; i++) {
        OpenFile *f = L[i];

        if (f->df > maximo)
            maximo = f->df;
    }

    for (long n = 0; n <= maximo; n++) {
        int df = (int)n;
        OpenFile *f = OpenFilesGet(df);

        if (f == NULL) {
            printf("descriptor: %d, offset: (  )-> no usado\n", df);
            continue;
        }

        off_t posicion = lseek(df, 0, SEEK_CUR);

        if (posicion == (off_t)-1) {
            printf("descriptor: %d, offset: (  )-> %s ",
                   df, f->name);
        }
        else {
            printf("descriptor: %d, offset: (%jd)-> %s ",
                   df, (intmax_t)posicion, f->name);
        }

        PrintMode(f->mode);
        printf("\n");
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
