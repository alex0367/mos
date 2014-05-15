#include <osdep.h>

#ifdef WIN32


void sema_init(semaphore* lock, char * name, int init_state)
{
	lock->event = CreateEventA(NULL, TRUE, (init_state == 0), name);
}

void sema_wait(semaphore* lock)
{
	WaitForSingleObject(lock->event, INFINITE);
}

void sema_trigger(semaphore* lock)
{
	SetEvent(lock->event);
}

void sema_reset(semaphore* lock)
{
	ResetEvent(lock->event);
}

static task_struct* task = NULL;
task_struct* CURRENT_TASK()
{
	if (!task){
		task = malloc(sizeof(*task));
		memset(task, 0, sizeof(*task));
		sema_init(&task->fd_lock, "fd_lock", 0);
	}

	return task;
}

#else


#endif

