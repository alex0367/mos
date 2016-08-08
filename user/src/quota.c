#include <klib.h>
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
    printf("kernel heap:\n\tcur %d\n\tmax %x\n\ttop %d\n", q.heap_cur, q.heap_wm, q.heap_top);
    printf("physical mm:\n\tcur %x\n\tmax %x\n\ttotal %h\n", q.phymm_cur, q.phymm_wm, q.phymm_max);
    printf("page table cache:\n\ttop %d\n\tcount %d\n", q.pgc_top, q.pgc_count);
    printf("total second: %d.%d, while sched use %d.%d of them\n", total_second, total_mili, sched_second, sched_milli);
    printf("total page fault count: %d\n", q.page_fault);
    return 0;
}

