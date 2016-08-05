#include <osdep.h>
#ifdef WIN32
#elif MACOS
#include <sys/types.h>
#include <dirent.h>
#endif

#include <fs/vfs.h>

#if defined WIN32 ||  MACOS
static unsigned quota = 0;
static unsigned high_quota = 0;
typedef struct _mm_block
{
    unsigned int len;
    char buf[0];
}mm_block;


//void* file_cache_find(char* path)
//{
//	return 0;
//}
//
//int file_cache_read(void* cachefd, unsigned off, void* buf, unsigned len)
//{
//	return 0;
//}

char *sys_getcwd(char *buf, unsigned size)
{
    strcpy(buf, "/");
    return buf;
}

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


    if ((prot & PROT_EXEC))
    {
        if ((prot & PROT_READ) && (prot & PROT_WRITE))
            protect = PAGE_EXECUTE_READWRITE;
        else if (prot & PROT_READ)
            protect = PAGE_EXECUTE_READ;
    }
    else if ((prot & PROT_READ) && (prot & PROT_WRITE))
    {
        protect = PAGE_READWRITE;
    }
    else if (((prot & PROT_READ) && !(prot & PROT_WRITE)))
    {
        protect = PAGE_READONLY;
    }
    else
    {
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
    return (VirtualFree(addr, len, MEM_DECOMMIT) == TRUE);
}

long __sync_add_and_fetch(long* v, long c)
{
    return InterlockedAdd(v, c);
}

void enum_dir(char* name, enum_dir_callback fn, char* dst_dir)
{
    WIN32_FIND_DATAA findData;
    HANDLE dir;
    char pwd[MAX_PATH + 1] = {0};

    GetCurrentDirectoryA(MAX_PATH, pwd);
    strcat(pwd, "/");
    strcat(pwd, name);
    strcat(pwd, "/*.*");
    dir = FindFirstFileA(pwd, &findData);
    if (dir != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!strcmp(findData.cFileName, "."))
                continue;
            if (!strcmp(findData.cFileName, ".."))
                continue;


            fn(findData.cFileName, dst_dir);
        }
        while (FindNextFileA(dir, &findData));
    }
}



#elif MACOS
void enum_dir(char* name, enum_dir_callback fn, char* dst_dir)
{
    DIR* d = opendir(name);
    struct dirent * entry;

    if (!d || !fn)
        return;

    while ((entry = readdir(d)))
    {
        if (!strcmp(entry->d_name, "."))
            continue;
        if (!strcmp(entry->d_name, ".."))
            continue;


        fn(entry->d_name, dst_dir);
    }

    closedir(d);

}

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

void spinlock_init(spinlock_t lock)
{
}

void spinlock_uninit(spinlock_t lock)
{
}

void spinlock_lock(spinlock_t lock)
{
}

void spinlock_unlock(spinlock_t lock)
{
}

static task_struct* task = NULL;
task_struct* CURRENT_TASK()
{
    if (!task)
    {
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
    char buf[256] = {0};
    char* tmp = buf;
    strcpy(tmp, "[mos]");
    tmp += strlen("[mos]");
    va_start(ap, msg);
    wvsprintfA(tmp, msg, ap);
    va_end(ap);

    OutputDebugStringA(buf);
}
#elif MACOS
#define printk printf
#endif

#else


#endif

