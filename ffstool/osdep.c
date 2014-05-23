#include <osdep.h>

#if defined WIN32 ||  MACOS
static unsigned quota = 0;
static unsigned high_quota = 0;
typedef struct _mm_block
{
	unsigned int len;
	char buf[0];
}mm_block;

void* _kmalloc(unsigned size, const char* function, int line)
{
	mm_block* ret = malloc(size + 4);
	//printf("%s:%d alloc %d \n", function, line, size);
	ret->len = size;
	quota += size;
	if (high_quota < quota)
		high_quota = quota;

	return ret->buf;
}

void _kfree(void* buf, const char* function, int line)
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

void* mmap(void* a, unsigned len, unsigned prot, unsigned flag, unsigned fd, unsigned pg_offset)
{
	unsigned addr = (unsigned)a;
	unsigned protect = 0;
	HANDLE h = fd;
	DWORD readed = 0;
	MEMORY_BASIC_INFORMATION info;


	if ( (prot & PROT_EXEC) ){
		if ((prot & PROT_READ) && (prot & PROT_WRITE))
			protect = PAGE_EXECUTE_READWRITE;
		else if (prot & PROT_READ)
			protect = PAGE_EXECUTE_READ;
	}
	else if ( (prot & PROT_READ) && (prot & PROT_WRITE)){
		protect = PAGE_READWRITE;
	}
	else if (((prot & PROT_READ) && !(prot & PROT_WRITE))){
		protect = PAGE_READONLY;
	}
	else{
		protect = PAGE_NOACCESS;
	}
	addr = addr & PAGE_SIZE_MASK;
	VirtualQuery(addr, &info, len);
	VirtualAlloc((void*)addr, len, MEM_RESERVE, protect);
	VirtualAlloc((void*)addr, len, MEM_COMMIT, protect);
	if (!addr)
	{
		DWORD err = GetLastError();
		return 0;
	}
	extern unsigned fs_read(unsigned fd, unsigned offset, void* buf, unsigned len);
	fs_read(fd, pg_offset, addr, len);
	return addr;
}

int munmap(void* addr, unsigned len)
{
	return ( VirtualFree(addr, len, MEM_DECOMMIT) == TRUE );
}

#elif MACOS
void sema_init(semaphore* lock, char * name, int init_state)
{
	// do nothing!
}

void sema_wait(semaphore* lock)
{
	// do nothing!
}

void sema_trigger(semaphore* lock)
{
	// do nothing!
}

void sema_reset(semaphore* lock)
{
	// do nothing!
}

unsigned GetCurrentTime()
{
    time_t now = time(NULL);
    return now;
}

#endif

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

#ifdef WIN32
void printk(char* msg, ...)
{
	va_list ap;
	char buf[256] = { 0 };

	va_start(ap, msg);
	wvsprintfA(buf, msg, ap);
	va_end(ap);

	OutputDebugStringA(buf);
}
#elif MACOS
#define printk printf
#endif

#else


#endif

