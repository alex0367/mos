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

    cur->user.heap_top = USER_HEAP_BEGIN;
}

static void ps_get_argc_envc(const char* file, 
                                 char** argv, char** envp, 
                                 unsigned* argv_len, unsigned* envp_len)
{
    char* tmp = 0;
    int i = 0;
    if (!argv_len || !envp_len) {
        return;
    }

    *argv_len = *envp_len = 0;
    *argv_len = *argv_len + 1;
    if (argv) {
        tmp = argv[i];
        while (tmp && *tmp) {
            *argv_len = *argv_len + 1;
            tmp = argv[++i];
        }
    }

    i = 0;
    if (envp) {
        tmp = envp[i];
        while (tmp && *tmp) {
            *envp_len = *envp_len + 1;
            tmp = envp[++i];
        }
    }    
}

static char** ps_save_argv(const char* file, char** argv, unsigned argc )
{
    char** ret = 0;
    int i = 0;
    if (!argc ) {
        return 0;
    }

    ret = kmalloc(argc*sizeof(char*));
    ret[0] = strdup(file);
    for (i = 0; i < (argc-1); i++) {
        ret[i+1] = strdup(argv[i]);
    }

    return ret;
}

static char** ps_save_envp(char** envp, unsigned envc)
{
    int i = 0;
    char** ret = 0;
    if (!envc) {
        return 0;
    }

    ret = kmalloc(envc*sizeof(char*));
    for (i = 0; i < envc; i++) {
        ret[i] = strdup(envp[i]);
    }

    return ret;

}

static void ps_free_v(char** v, unsigned size)
{
    int i = 0;
    if (!v) {
        return;
    }

    for (i = 0; i < size; i++) {
        kfree(v[i]);
    }
    kfree(v);
}

static unsigned ps_setup_v(unsigned argc, char** argv, unsigned envc, char** envp, unsigned top)
{
    int i = 0;
    char* esp = (char*)(top - 1);
    int argv_buf_len = argc * sizeof(char*);
    int env_buf_len = envc * sizeof(char*);
    char** tmp_array_argv = 0;
    char** tmp_array_env = 0;
    unsigned argvp, envpp;

    if (1) {
        tmp_array_argv = kmalloc(argv_buf_len+4);
    }

    if (1) {
        tmp_array_env = kmalloc(env_buf_len+4);
    }

    *esp = '\0'; 
    for (i = 0; i < argc; i++) {
        int len = strlen(argv[i])+1;
        esp -= len;
        strcpy(esp, argv[i]);
        tmp_array_argv[i] = esp;
        esp[len-1] = '\0';
    }

    tmp_array_argv[argc] = 0;

    esp--;
    *esp = '\0';
    for (i = 0; i < envc; i++) {
        int len = strlen(envp[i])+1;
        esp -= len;
        strcpy(esp, envp[i]);
        tmp_array_env[i] = esp;
        esp[len-1] = '\0';
    }

    tmp_array_env[envc] = 0;


    if (1) {
        esp -= (env_buf_len+4); 
        envpp = esp;
        memcpy(envpp, tmp_array_env, env_buf_len+4);
    }

    if (1) {
        esp -= (argv_buf_len+4); 
        argvp = esp;
        memcpy(argvp, tmp_array_argv, argv_buf_len+4);
    }



    if (1) {
        kfree(tmp_array_argv);
    }

    if (1) {
        kfree(tmp_array_env);
    }


    esp -= 4;
    *((unsigned*)esp) = argc;

    //esp -= 4;
    //*((unsigned*)esp) = 0;

    return esp;

}

int sys_execve(const char* file, char** argv, char** envp)
{
    unsigned eip = 0;
    int i = 0;
    unsigned esp_buttom = KERNEL_OFFSET - USER_STACK_PAGES*PAGE_SIZE;
    unsigned esp_top = KERNEL_OFFSET;
    char file_name[64] = {0};
    unsigned argc = 0, envc = 0;
    char** s_argv = 0;
    char** s_envp = 0;

    if (!file) {
        printk("fatal error: trying to execvp empty file!\n");
        return -1;
    }

    ps_get_argc_envc(file,argv,envp,&argc,&envc);
    s_argv = ps_save_argv(file,argv, argc);
    s_envp = ps_save_envp(envp, envc);

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


    esp_top = ps_setup_v(argc,s_argv,envc,s_envp,esp_top);

    ps_free_v(s_argv, argc);
    ps_free_v(s_envp, envc);

    extern void switch_to_user_mode(unsigned eip, unsigned esp);
    switch_to_user_mode(eip, esp_top);
    // never return here
    return 0;
}

static void user_setup_enviroment()
{
    unsigned esp0 = (unsigned)CURRENT_TASK() + PAGE_SIZE;
    ps_update_tss(esp0);
    sys_execve("/bin/run", 0, 0);
}



void user_first_process_run()
{
    user_setup_enviroment();

}
