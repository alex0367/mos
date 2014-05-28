#include <syscall.h>
#include <fs.h>
#include <syscall/unistd.h>

#define SH_PREFIX "sh$ "

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
		printf("%s: command not found\n", cmd);
		return;
	}

	if (S_ISDIR(s.st_mode)){
		printf("%s: command not found\n", cmd);
		return;
	}

	pid = fork();
	if (pid){
		waitpid(pid, 0, 0);
	}else{
        char* argv[2];
		if (arg_line) {
			argv[0] = arg_line;
			argv[1] = '\0';
			execve(path, argv, 0);
		}else{
			execve(path, 0, 0);
		}
		
	}

}

void main()
{
	char cmd[80] = {0};
	int idx = 0;
	char* tmp = cmd;
	printf(SH_PREFIX);
	while(1){
		read(0, tmp, 1);
		write(1, tmp, 1);
		if( *tmp == '\r' ){
			char* args = 0;

            *tmp = '\0';
			printf("%s%s\n", SH_PREFIX, cmd);
			args = strchr(cmd, ' ');
			if (args) {
				*args = '\0';
				args++;
			}
			run_cmd(cmd, args);
			printf(SH_PREFIX);
			idx = 0;
			tmp = cmd;
		}else{
			idx++;
			tmp++;
		}
	}
}
