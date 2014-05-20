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
#elif MACOS
#define kmalloc(size) _kmalloc(size, __func__, __LINE__)
#define kfree(buf) _kfree(buf, __func__, __LINE__)
#endif


typedef struct _task_struct
{
	unsigned fds[256];
	semaphore fd_lock;
}task_struct;

task_struct* CURRENT_TASK();
#define MAX_FD 256
#define UNIMPL 

void printk(char* msg, ...);

#define vmalloc(page_count) malloc(page_count*4096)
#define vmfree(addr, page_count) free(addr)


#endif
