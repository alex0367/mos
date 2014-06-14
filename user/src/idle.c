#include <syscall.h>

void main()
{
	int i = 0;
	int pid = -1;
	int psid = -1;

	psid = fork();
	if (psid){ // parent
	
	}else{ // child
        char* argv[2];
        argv[0] = "/bin/run";
        argv[1] = 0;
		execve("/bin/run", argv, 0);		
	}
	
	while(1){
		pause();
        sched_yield();
    }
}
