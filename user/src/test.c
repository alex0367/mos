#include <syscall.h>
#include <lib/klib.h>

#define MIN_MEM 1
#define MAX_MEM 4080
#define ALLOC_COUNT 50

void malloc_test()
{
    void* addr[ALLOC_COUNT];
    int size = 0;
    int mallocor = 10;
    int loop = 100000;
    int c = 0;
    srand(time(0));


    int* a = malloc(1);
    int* b = malloc(25);

    free(a);
    free(b);
    //int* tmp = (int*)kmalloc(sizeof(int));
    //*tmp = 0xdeadbeef;


    //while (loop-- > 0){
    //	for (int i = 0; i < sizeof(addr) / sizeof(*addr); i++){
    //		size = rand() % 4096;
    //		if (size == 0)
    //			continue;
    //		addr[i] = kmalloc(size);
    //		assert(addr[i] != NULL);
    //		memset(addr[i], 0, size);
    //		kfree(addr[i]);
    //	}
    //}

    loop = 100;
    while (1)
    {
        loop--;
        if (loop <= 0)
        {
            printf("%d: I'm still alive, now top %x\n", c++, brk(0));
            loop = 100;
        }
        int mem_count = rand() % ALLOC_COUNT;
        int i = 0;
        if (mem_count == 0)
            mem_count = 1;
        for (i = 0; i < mem_count; i++)
        {
            size = rand() % (MAX_MEM - MIN_MEM);
            size += MIN_MEM;
            if (size == 0)
                addr[i] = 0;
            else
            {
                addr[i] = malloc(size);
                if (addr[i] == 0)
                {
                    printf("hello");
                }
                memset(addr[i], 0xc, size);
            }
        }

        for (i = 0; i < mem_count; i++)
        {
            free(addr[i]);
        }
    }
    //assert(*tmp == 0xdeadbeef);
    //kfree(tmp);

}

int main()
{
    malloc_test();
    return 1;
}
