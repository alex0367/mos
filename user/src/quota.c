#include <lib/klib.h>
#include <syscall.h>

int main()
{
	struct krnquota q;

	quota(&q);

	printf("kernel heap:\tcur %d, max %d\n", q.heap_cur, q.heap_wm);
	printf("physical mm:\tcur %x, max %x, total %h\n", q.phymm_cur, q.phymm_wm, q.phymm_max);
	return 0;
}

