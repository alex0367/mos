#include <lib/klib.h>
#include <syscall.h>

int main()
{
	struct krnquota q;
	unsigned total_second, total_mili, sched_second, sched_milli;
	quota(&q);
	
	total_second = q.total_spent / 1000;
	total_mili = q.total_spent % 1000;
	sched_second = q.sched_spent / 1000;
	sched_milli = q.sched_spent % 1000;
	printf("kernel heap:\tcur %h, max %h, top %x\n", q.heap_cur, q.heap_wm, q.heap_top);
	printf("physical mm:\tcur %h, max %h, total %h\n", q.phymm_cur, q.phymm_wm, q.phymm_max);
	printf("page table cache:\ttop %x, count %d\n", q.pgc_top, q.pgc_count);
	printf("total second: %d.%d, while sched use %d.%d of them\n", total_second, total_mili, sched_second, sched_milli);
	return 0;
}

