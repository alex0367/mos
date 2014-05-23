#include <syscall.h>

void main()
{
	int ret;
	char c[1];
	while(read(0, c, 1) == 1){
		write(1, c, 1);
	}
}
