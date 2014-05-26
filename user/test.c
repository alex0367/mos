#include <syscall.h>

void main()
{
	char msg[] = "hello, world from test!!\n";
	write(1, msg, sizeof(msg)-1);
	while(1);
}
