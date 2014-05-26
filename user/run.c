#include <syscall.h>

void main()
{
	int i = 0;
	int pid = -1;
	int psid = -1;
	char msg[] = "ps[9]: hello, world from user land\n";

	psid = fork();
	if (psid){ // parent
	
	}else{ // child
		execve("/bin/sh", 0, 0);		
	}
	/*char msg1[] = "return val 1\n";
	while(i < 3){
		psid = fork();
		i++;	
	}
	*/
	//write(1, "return\n",7);
	//while(1); 
	/*if (psid){ // parent
		while(1)
			write(1, "parent\n", 7);
	}else{
		while(1)
			write(1, "child \n", 7);
	}*/

	//while(1);
	
	pid = getpid();
	msg[3] = pid+'0';
	write(1, msg, sizeof(msg)-1);
	while(1){
        sched_yield();
    }
}
