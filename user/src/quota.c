#include <lib/klib.h>
#include <syscall.h>

int main()
{
	struct krnquota q;

	quota(&q);

	printf("kernel heap:\tcur %d, max %d, top %x\n", q.heap_cur, q.heap_wm, q.heap_top);
	printf("physical mm:\tcur %h, max %h, total %h\n", q.phymm_cur, q.phymm_wm, q.phymm_max);
	return 0;
}

