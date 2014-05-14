#include <user/ps0.h>
#include <ps/ps.h>
#include <int/int.h>
#include <lib/klib.h>
#include <mm/mm.h>

_USER static void user_process()
{
	int i = 0;
	while (1)
	{
		i++;
		if (i == 100)
		{
			//__asm__("int $3");
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
	user_setup_enviroment(0x00804000, 0x00800000);
	extern switch_to_user_mode();
	switch_to_user_mode();
}
