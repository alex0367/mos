#include <syscall.h>
#include <lib/klib.h>

int main(int argc, char** argv)
{
	char name[256] = {0};

	getcwd(name, 256);

	printf("%s\n", name);

	return 1;
}

