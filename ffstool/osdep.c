#include <osdep.h>

#ifdef WIN32
static unsigned quota = 0;
static unsigned high_quota = 0;
typedef struct _mm_block
{
	unsigned int len;
	char buf[0];
}mm_block;

void* _kmalloc(unsigned size, char* function, int line)
{
	mm_block* ret = malloc(size + 4);
	//printf("%s:%d alloc %d \n", function, line, size);
	ret->len = size;
	quota += size;
	if (high_quota < quota)
		high_quota = quota;

	return ret->buf;
}

void _kfree(void* buf, char* function, int line)
{
	mm_block* b = (char*)buf - 4;
	//printf("%s:%d free %d \n", function, line, b->len);
	quota -= b->len;
	free(b);
}

void print_quota()
{
	printf("quota %x, highest %x\n", quota, high_quota);
}


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

void printk(char* msg, ...)
{
	va_list ap;
	char buf[256] = { 0 };

	va_start(ap, msg);
	wvsprintfA(buf, msg, ap);
	va_end(ap);

	OutputDebugStringA(buf);
}

#else


#endif

