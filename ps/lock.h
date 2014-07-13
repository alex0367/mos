#ifndef _LOCK_H_
#define _LOCK_H_
#include <lib/list.h>

typedef struct _spinlock
{
	unsigned int lock;
	int inited;
	unsigned int int_status;
	unsigned holder;
}spinlock, *spinlock_t;

void spinlock_init(spinlock_t lock);

void spinlock_uninit(spinlock_t lock);

void spinlock_lock(spinlock_t lock);

void spinlock_unlock(spinlock_t lock);

// semaphore

typedef struct _semaphore
{
	char name[16];
	unsigned int lock;
}semaphore;

void sema_init(semaphore* s, const char* name, unsigned int initstat);

void sema_wait(semaphore* s);

void sema_reset(semaphore* s);

void sema_trigger(semaphore* s);

void sema_trigger_at_intr(semaphore* s);

#endif
