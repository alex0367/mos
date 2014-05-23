#include <user/ps0.h>
#include <ps/ps.h>
#include <int/int.h>
#include <lib/klib.h>
#include <ps/elf.h>
#include <mm/mm.h>
#include <config.h>



static void user_setup_enviroment()
{
    unsigned eip = 0;
    int i = 0;
    unsigned esp_buttom = KERNEL_OFFSET - USER_STACK_PAGES*PAGE_SIZE;
    unsigned esp_top = KERNEL_OFFSET - 4;

    eip = elf_map("/bin/run");
    if (!eip) {
        printk("fatal error: no idle task found!\n");
        asm("hlt");
    }

    for (i = 0; i < USER_STACK_PAGES; i++) {
        mm_add_dynamic_map(esp_buttom+i*PAGE_SIZE, 0, PAGE_ENTRY_USER_DATA);
    }

    extern void switch_to_user_mode(unsigned eip, unsigned esp);
	switch_to_user_mode(eip, esp_top);

}



void user_first_process_run()
{
	unsigned int esp0;
    __asm__("movl %%esp, %0" : "=r"(esp0));
	ps_update_tss(esp0);

	user_setup_enviroment();

}
