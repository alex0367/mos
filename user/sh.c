#include <syscall.h>
#include <fs.h>
#include <syscall/unistd.h>
#include <lib/klib.h>

#define SH_PREFIX "sh$ "
struct utsname machine;

static void print_sh_prefix(int status)
{
	char cwd[256] = {0};
	getcwd(cwd, 256);
	tty_set_fg_color(clGreen);
	printf("%s@%s ", "root", "root");
	tty_set_fg_color(clBlue);
	printf("%s\n", cwd);
	if (status >= 0)
	  tty_set_fg_color(clBlue);
	else
	  tty_set_fg_color(clRed);
	printf("$ ");
	tty_set_fg_color(clLightGray);
}

static int run_cmd(char* cmd, char* arg_line)
{
	char path[64] = "/bin/";
	struct stat s;
	int pid = 0;
	int status = 0;

	if (!cmd || !*cmd)
	  return 0;

    if (!strcmp(cmd, "exit")) {
        exit(0);
    }

	if (!strcmp(cmd, "cd")){
		if (!chdir(arg_line)){
			printf("No such directory!\n");
			return -1;
		}
		return 0;
	}

    strcat(path, cmd); 
	if (stat(path, &s) == -1){
		printf("%s: command not found\n", cmd);
		return -1;
	}

	if (S_ISDIR(s.st_mode)){
		printf("%s: command not found\n", cmd);
		return -1;
	}

	pid = fork();
	if (pid){
		status = waitpid(pid, 0, 0);
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
	return status;
}

void main()
{
	char cmd[80] = {0};
	int idx = 0;
	char* tmp = cmd;

	uname(&machine);

	print_sh_prefix(0);
	while(1){
		read(0, tmp, 1);
		if( *tmp == '\r' ){
			char* args = 0;
			int status = 0;

            *tmp = '\0';
			write(1, tmp, 1);
			write(1, "\n", 1);
			args = strchr(cmd, ' ');
			if (args) {
				*args = '\0';
				args++;
			}
			status = run_cmd(cmd, args);
			print_sh_prefix(status);
			idx = 0;
			tmp = cmd;
		}else{
			write(1, tmp, 1);
			idx++;
			tmp++;
		}
	}
}
