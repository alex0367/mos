#include <syscall.h>

void main()
{
	int ret;
    int pid = getpid();
	char msg[] = "ps[0]: hello, world from user land\n";
    msg[3] = pid+'0';
	while(1)
		write(1, msg, sizeof(msg));
}
