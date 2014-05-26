#include <syscall.h>


static void run_cmd(char* cmd)
{
}

void main()
{
	char cmd[80] = {0};
	int idx = 0;
	char* tmp = cmd;
	write(1, "sh:", 3); 
	while(1){
		read(0, tmp, 1);
		write(1, tmp, 1);
		if( *tmp == '\r' ){
			write(1, "sh:", 3);
			write(1, cmd, idx);
			write(1, "\n", 1);
			run_cmd(cmd);
			write(1, "sh:", 3);
			idx = 0;
			tmp = cmd;
		}else{
			idx++;
			tmp++;
		}
	}
}
