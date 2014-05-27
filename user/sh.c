#include <syscall.h>
#include <fs.h>
#include <syscall/unistd.h>

#define SH_PREFIX "sh$ "
#define COMMAND_NOT_FOUND ": command not found\n"

static void run_cmd(char* cmd, char* arg_line)
{
	char path[64] = "/bin/";
	struct stat s;
	int pid = 0;

    if (!strcmp(cmd, "exit")) {
        exit(0);
    }
    strcat(path, cmd); 
	if (stat(path, &s) == -1){
		write(1, cmd, strlen(cmd));
		write(1, COMMAND_NOT_FOUND, strlen(COMMAND_NOT_FOUND));
		return;
	}

	if (S_ISDIR(s.st_mode)){
		write(1, cmd, strlen(cmd));
		write(1, COMMAND_NOT_FOUND, strlen(COMMAND_NOT_FOUND));
		return;
	}

	pid = fork();
	if (pid){
		waitpid(pid, 0, 0);
	}else{
        char* argv[2];
        argv[0] = "-l";
        argv[1] = "\0";
		execve(path, argv, 0);
	}

}

void main()
{
	char cmd[80] = {0};
	int idx = 0;
	char* tmp = cmd;
	write(1, SH_PREFIX, strlen(SH_PREFIX));
	while(1){
		read(0, tmp, 1);
		write(1, tmp, 1);
		if( *tmp == '\r' ){
            *tmp = '\0';
			write(1, SH_PREFIX, strlen(SH_PREFIX));
			write(1, cmd, idx);
			write(1, "\n", 1);
			run_cmd(cmd, 0);
			write(1, SH_PREFIX, strlen(SH_PREFIX));
			idx = 0;
			tmp = cmd;
		}else{
			idx++;
			tmp++;
		}
	}
}
