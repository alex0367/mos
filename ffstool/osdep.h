#ifndef OSDEP_H
#define OSDEP_H


#ifdef WIN32
#include <windows.h>
#include <lib/list.h>

typedef struct _semaphore
{
	HANDLE event;

}semaphore;

void sema_init(semaphore* lock, char * name, int init_state);

void sema_wait(semaphore* lock);

void sema_trigger(semaphore* lock);

void sema_reset(semaphore* lock);

#define kmalloc malloc
#define kfree free

typedef struct _task_struct
{
	unsigned fds[256];
	semaphore fd_lock;
}task_struct;

task_struct* CURRENT_TASK();
#define MAX_FD 256
#define UNIMPL 


#else


#endif

#endif