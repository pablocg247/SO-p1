#ifndef MEMLIST_H
#define MEMLIST_H

#include "listasimple.h"
#include "openfileslist.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>

typedef enum {
	MEM_ALL = -1,
	MEM_MALLOC = 0,
	MEM_SHARED = 1,
	MEM_MMAP = 2
} MemType;

typedef struct MemBlock {
	void *addr;
	size_t size;
	time_t time;
	MemType type;
	key_t key;                /* Solo para MEM_SHARED */
	char name[MAXFILENAME];   /* Solo para MEM_MMAP */
	int df;                   /* Solo para MEM_MMAP */
} MemBlock;

int MemListAddMalloc(void *addr, size_t size);
int MemListAddShared(void *addr, size_t size, key_t key);
int MemListAddMmap(void *addr, size_t size, const char *name, int df);

MemBlock *MemListGetByAddr(void *addr);
MemBlock *MemListGetByMallocSize(size_t size);
MemBlock *MemListGetBySharedKey(key_t key);
MemBlock *MemListGetByMmapFile(const char *name);
int MemListHasMappedFd(int df);
void MemListForgetFd(int df);

int MemListDelByAddr(void *addr);
void MemListPrint(MemType filter);
void MemListClear(void);

#endif
