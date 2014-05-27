#include <syscall.h>

void start(int argc, char** argv, char** envp)
{
	unsigned status;

	extern int main(int argc, char** argv);

	status = main(argc, argv);

	exit(status);
}
