#include <syscall.h>

void main()
{
	int i = 0;
	int pid = -1;
	int psid = -1;

	psid = fork();
	if (psid){ // parent
	
	}else{ // child
		execve("/bin/sh", 0, 0);		
	}
	
	while(1){
		int status = 0;
		int pid = waitpid(0, &status, 0);
		sched_yield();
    }
}
