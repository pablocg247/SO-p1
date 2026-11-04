#include "memlist.h"

static LISTASIMPLE M;

static int MemListAdd(void *addr, size_t size, MemType type, key_t key, const char *name, int df)
{
    MemBlock *b = malloc(sizeof(MemBlock));
    if (b == NULL) {
        errno = ENOMEM;
        return -1;
    }

    b->addr = addr;
    b->size = size;
    b->time = time(NULL);
    b->type = type;
    b->key = key;
    b->df = df;

    if (name != NULL) {
        strncpy(b->name, name, sizeof(b->name) - 1);
        b->name[sizeof(b->name) - 1] = '\0';
    } else {
        b->name[0] = '\0';
    }

    int res = AniadirElemento(M, b);
    if (res == -1) {
        free(b);
        return -1;
    }
    return res;
}

int MemListAddMalloc(void *addr, size_t size)
{
    return MemListAdd(addr, size, MEM_MALLOC, 0, NULL, -1);
}

int MemListAddShared(void *addr, size_t size, key_t key)
{
    return MemListAdd(addr, size, MEM_SHARED, key, NULL, -1);
}

int MemListAddMmap(void *addr, size_t size, const char *name, int df)
{
    return MemListAdd(addr, size, MEM_MMAP, 0, name, df);
}

MemBlock *MemListGetByAddr(void *addr)
{
    for (int i = 0; i < MAXLISTASIMPLE && M[i] != NULL; i++) {
        MemBlock *b = (MemBlock *)M[i];
        if (b->addr == addr)
            return b;
    }
    return NULL;
}

MemBlock *MemListGetByMallocSize(size_t size)
{
    for (int i = 0; i < MAXLISTASIMPLE && M[i] != NULL; i++) {
        MemBlock *b = (MemBlock *)M[i];
        if (b->type == MEM_MALLOC && b->size == size)
            return b;
    }
    return NULL;
}

MemBlock *MemListGetBySharedKey(key_t key)
{
    for (int i = 0; i < MAXLISTASIMPLE && M[i] != NULL; i++) {
        MemBlock *b = (MemBlock *)M[i];
        if (b->type == MEM_SHARED && b->key == key)
            return b;
    }
    return NULL;
}

MemBlock *MemListGetByMmapFile(const char *name)
{
    for (int i = 0; i < MAXLISTASIMPLE && M[i] != NULL; i++) {
        MemBlock *b = (MemBlock *)M[i];
        if (b->type == MEM_MMAP && !strcmp(b->name, name))
            return b;
    }
    return NULL;
}

int MemListHasMappedFd(int df)
{
    for (int i = 0; i < MAXLISTASIMPLE && M[i] != NULL; i++) {
        MemBlock *b = (MemBlock *)M[i];
        if (b->type == MEM_MMAP && b->df == df)
            return 1;
    }
    return 0;
}

int MemListDelByAddr(void *addr)
{
    for (int i = 0; i < MAXLISTASIMPLE && M[i] != NULL; i++) {
        MemBlock *b = (MemBlock *)M[i];
        if (b->addr == addr)
            return BorrarElementoAtPos(M, i);
    }
    errno = ENOENT;
    return -1;
}

void MemListPrint(MemType filter)
{
    const char *tipo_cabecera = "";

    if (filter == MEM_MALLOC)
        tipo_cabecera = " malloc";
    else if (filter == MEM_SHARED)
        tipo_cabecera = " shared";
    else if (filter == MEM_MMAP)
        tipo_cabecera = " mmap";

    printf("******Lista de bloques asignados%s para el proceso %lu\n",
           tipo_cabecera, (unsigned long)getpid());

    for (int i = 0; i < MAXLISTASIMPLE && M[i] != NULL; i++) {
        MemBlock *b = (MemBlock *)M[i];

        if (filter != MEM_ALL && b->type != filter)
            continue;

        /* Formatear la fecha usando ctime sin el salto de linea final */
        char fecha[32] = "desconocida";
        char *ct = ctime(&b->time);
        if (ct != NULL) {
            /* ctime devuelve "Day Mon DD HH:MM:SS YYYY\n\0";
               en el shell de referencia se salta el dia de la semana (+4) y el anio */
            snprintf(fecha, sizeof(fecha), "%.12s", ct + 4);
        }

        printf("%20p  %15llu %s",
               b->addr,
               (unsigned long long)b->size,
               fecha);

        if (b->type == MEM_MALLOC) {
            printf("  malloc\n");
        } else if (b->type == MEM_SHARED) {
            printf("  shared (key %lu)\n", (unsigned long)b->key);
        } else if (b->type == MEM_MMAP) {
            printf("  %s  (descriptor %ld)\n", b->name, (long)b->df);
        } else {
            printf("   unknown\n");
        }
    }
}

void MemListClear(void)
{
    for (int i = 0; i < MAXLISTASIMPLE && M[i] != NULL; i++) {
        MemBlock *b = (MemBlock *)M[i];
        if (b->type == MEM_MALLOC) {
            free(b->addr);
        } else if (b->type == MEM_SHARED) {
            shmdt(b->addr);
        } else if (b->type == MEM_MMAP) {
            munmap(b->addr, b->size);
        }
    }
    BorrarLista(M);
}