#ifndef OSDEP_H
#define OSDEP_H


#ifdef WIN32
#include <windows.h>
#include <lib/list.h>
#include <errno.h>
typedef struct _semaphore
{
	HANDLE event;

}semaphore;
#elif MACOS
#include <lib/list.h>
//#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syscall/unistd.h>
typedef struct _semaphore
{
	unsigned long long event;

}semaphore;

unsigned GetCurrentTime();
#endif

typedef void (*enum_dir_callback)(char* name, char* dst_dir);

void enum_dir(char* dir, enum_dir_callback fn, char* dst_dir);

void sema_init(semaphore* lock, char * name, int init_state);

void sema_wait(semaphore* lock);

void sema_trigger(semaphore* lock);

void sema_reset(semaphore* lock);

void* _kmalloc(unsigned size, const char* f, int line);
void _kfree(void* buf, const char* f, int line);

#ifdef WIN32
#define kmalloc(size) malloc(size)
#define kfree(buf) free(buf)
void* mmap(void* addr, unsigned len, unsigned prot, unsigned flag, unsigned fd, unsigned pg_offset);
int munmap(void* addr, unsigned len);
#elif MACOS
#define kmalloc(size) malloc(size)
#define kfree(buf) free(buf)
#include <sys/mman.h>
#endif

#define fd_flag_isdir 0x00000001
#define fd_flag_readonly 0x00000002
#define fd_flag_create 0x00000004
#define fd_flag_append 0x00000008
#define fd_flag_used	0x80000000

typedef struct _spinlock
{
    int dummy;
}spinlock, *spinlock_t;

typedef struct _fd_type
{
	union{
		unsigned	file;
		unsigned		dir;
	};
	unsigned file_off;
	unsigned flag;
	char *path;
}fd_type;
typedef struct _task_struct
{

	fd_type fds[256];
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

void* file_cache_find(char* path);

int file_cache_read(void* cachefd, unsigned off, void* buf, unsigned len);


char *sys_getcwd(char *buf, unsigned size);

#endif
