#ifndef OSDEP_H
#define OSDEP_H


#ifdef WIN32
#include <windows.h>
#include <lib/list.h>
typedef struct _semaphore
{
	HANDLE event;

}semaphore;
#elif MACOS
#include <lib/list.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct _semaphore
{
	unsigned long long event;

}semaphore;

unsigned GetCurrentTime();
#endif



void sema_init(semaphore* lock, char * name, int init_state);

void sema_wait(semaphore* lock);

void sema_trigger(semaphore* lock);

void sema_reset(semaphore* lock);

void* _kmalloc(unsigned size, const char* f, int line);
void _kfree(void* buf, const char* f, int line);

#ifdef WIN32
#define kmalloc(size) _kmalloc(size, __FUNCTION__, __LINE__)
#define kfree(buf) _kfree(buf, __FUNCTION__, __LINE__)
void* mmap(void* addr, unsigned len, unsigned prot, unsigned flag, unsigned fd, unsigned pg_offset);
int munmap(void* addr, unsigned len);
#elif MACOS
#define kmalloc(size) _kmalloc(size, __func__, __LINE__)
#define kfree(buf) _kfree(buf, __func__, __LINE__)
#include <sys/mman.h>
#endif


typedef struct _task_struct
{
	unsigned fds[256];
	semaphore fd_lock;
}task_struct;

task_struct* CURRENT_TASK();
#define MAX_FD 256
#define UNIMPL 
#define PAGE_SIZE_MASK 0xFFFFF000
#define PAGE_SIZE (4*1024)
#define ROUND_UP(X, STEP) (((X) + (STEP) - 1) / (STEP) * (STEP))
#define PROT_READ   0x1     /* page can be read */
#define PROT_WRITE  0x2     /* page can be written */
#define PROT_EXEC   0x4     /* page can be executed */
#define PROT_SEM    0x8     /* page may be used for atomic ops */
#define PROT_NONE   0x0     /* page can not be accessed */
#define PROT_GROWSDOWN  0x01000000  /* mprotect flag: extend change to start of growsdown vma */
#define PROT_GROWSUP    0x02000000  /* mprotect flag: extend change to end of growsup vma */

#define MAP_SHARED  0x01        /* Share changes */
#define MAP_PRIVATE 0x02        /* Changes are private */
#define MAP_TYPE    0x0f        /* Mask for type of mapping (OSF/1 is _wrong_) */
#define MAP_FIXED   0x100       /* Interpret addr exactly */
#define MAP_ANONYMOUS   0x10        /* don't use a file */

void printk(char* msg, ...);

#define vmalloc(page_count) malloc(page_count*4096)
#define vmfree(addr, page_count) free(addr)


#endif
