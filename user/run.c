#include <syscall.h>

void main()
{
	int ret;
	int pid = -1;
	int psid = -1;
	char msg[] = "ps[9]: hello, world from user land\n";
	// msg[3] = pid+'0';
	psid = fork();
	write(1, "return\n",7);
	while(1); 
	if (psid){ // parent
		while(1)
			write(1, "parent\n", 7);
	}else{
		while(1)
			write(1, "child \n", 7);
	}

	//while(1);
	
	pid = getpid();
	msg[3] = pid+'0';
	while(1)
		write(1, msg, sizeof(msg));
}
