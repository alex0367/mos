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
	printf("kernel heap:\n\tcur %x\n\tmax %x\n\ttop %x\n", q.heap_cur, q.heap_wm, q.heap_top);
	printf("physical mm:\n\tcur %h\n\tmax %h\n\ttotal %h\n", q.phymm_cur, q.phymm_wm, q.phymm_max);
	printf("page table cache:\n\ttop %x\n\tcount %d\n", q.pgc_top, q.pgc_count);
	printf("total second: %d.%d, while sched use %d.%d of them\n", total_second, total_mili, sched_second, sched_milli);
	return 0;
}

