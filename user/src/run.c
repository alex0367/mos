#include <syscall.h>

static char* sh;
static char* argv[2];

void main()
{
	int i = 0;
	int pid = -1;
	int psid = -1;
    struct stat s;

	psid = fork();
	if (psid){ // parent
	
	}else{ // child
        if (stat("/bin/bash", &s) != -1) {
            sh = "/bin/bash";
            argv[0] = "/bin/bash";
        }else if(stat("/bin/sh", &s) != -1) {
            sh = "/bin/sh";
            argv[1] = "/bin/sh";
        }else{
            printf("No sh!\n");
            exit(-1);
        }
        argv[1] = 0;
        execve(sh, argv, 0);		
	}
	
	while(1){
		int status = 0;
		int pid = waitpid(0, &status, 0);
		if (pid == psid){
			printf("Fatal error: first process exit!\n");
			psid = fork();
			if(!psid){
				execve(sh, argv, 0);
			}
		}
        pause();
		sched_yield();
    }
}
