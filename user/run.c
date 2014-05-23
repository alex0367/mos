#include <syscall.h>

void main()
{
	int ret;
	char* msg = "hello, world from user land\n";
	while(1)
		write(1, msg, 14);
}
