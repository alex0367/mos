#include <user/ps0.h>
#include <ps/ps.h>
#include <int/int.h>
#include <lib/klib.h>
#include <ps/elf.h>
#include <mm/mm.h>
#include <config.h>

static void cleanup()
{
    task_struct* cur = CURRENT_TASK();
    int i = 0;

    for (i = 0; i < MAX_FD; i++) {
        if (cur->fds[i] != 0 && 
            cur->fds[i] != INODE_STD_IN &&
            cur->fds[i] != INODE_STD_OUT &&
            cur->fds[i] != INODE_STD_ERR) {
            fs_close(cur->fds[i]);
            cur->file_off[i] = 0;
        }
    }

    ps_cleanup_all_user_map(cur);
}

int sys_execve(const char* file, char** argv, char** envp)
{
    unsigned eip = 0;
    int i = 0;
    unsigned esp_buttom = KERNEL_OFFSET - USER_STACK_PAGES*PAGE_SIZE;
    unsigned esp_top = KERNEL_OFFSET - 4;
    
    char file_name[64] = {0};
    strcpy(file_name, file);

    cleanup();

    eip = elf_map(file_name);
    if (!eip) {
        printk("fatal error: file %s not found!\n", file);
        asm("hlt");
    }

    for (i = 0; i < USER_STACK_PAGES; i++) {
        mm_add_dynamic_map(esp_buttom+i*PAGE_SIZE, 0, PAGE_ENTRY_USER_DATA);
    }

    extern void switch_to_user_mode(unsigned eip, unsigned esp);
    switch_to_user_mode(eip, esp_top);
    // never return here
    return 0;
}

static void user_setup_enviroment()
{
    unsigned int esp0;
    __asm__("movl %%esp, %0" : "=r"(esp0));
    ps_update_tss(esp0);
    sys_execve("/bin/run", 0, 0);
}



void user_first_process_run()
{
    user_setup_enviroment();

}
