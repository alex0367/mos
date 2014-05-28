#include <syscall.h>
#include <lib/klib.h>

void start(int argc, char** argv, char** envp)
{
	unsigned status;

	libc_init();

	extern int main(int argc, char** argv);

	status = main(argc, argv);

	exit(status);
}
