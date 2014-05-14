#include <user/ps0.h>
#include <ps/ps.h>
#include <int/int.h>
#include <lib/klib.h>
#include <mm/mm.h>


_USER static void user_process()
{
	int i = 0;
	char str[6];
	unsigned addr = (unsigned)str;

	while (1)
	{
		i++;
		if (i == 100)
		{
			// FIXME
			// no function call can perform here
			// because address in "call xxx" is in kernel space
			// formal way should using elf loader
			str[0] = 'h';
			str[1] = 'e';
			str[2] = 'l';
			str[3] = 'l';
			str[4] = 'o';
			str[5] = '\0';
			__asm__("movl $0, %ebx");
			__asm__("movl %0, %%ecx" : : "m"(addr) );
			__asm__("movl $6, %edx");
			__asm__("movl $4, %eax");
			__asm__("int $0x80");

		}
	}
}

static void user_setup_enviroment(unsigned int code_addr, unsigned int stack_addr)
{
	task_struct* cur = CURRENT_TASK();


	code_addr  = code_addr  & PAGE_SIZE_MASK;
	stack_addr = stack_addr & PAGE_SIZE_MASK;

	mm_add_dynamic_map(code_addr, 
					   (unsigned int)user_process - KERNEL_OFFSET, 
					   PAGE_ENTRY_USER_DATA);
	ps_record_dynamic_map(code_addr);

	while (stack_addr < code_addr)
	{
		mm_add_dynamic_map(stack_addr, 0, PAGE_ENTRY_USER_DATA);
		ps_record_dynamic_map(stack_addr);
		stack_addr += PAGE_SIZE;
	}
}



void user_first_process_run()
{
	unsigned int esp0;
	user_setup_enviroment(0x00804000, 0x00800000);
	__asm__("movl %%esp, %0" : "=r"(esp0));
	ps_update_tss(esp0);
	extern switch_to_user_mode();
	switch_to_user_mode();
}
