#include <syscall.h>

#define SH_PREFIX "sh$ "
#define COMMAND_NOT_FOUND ": command not found\n"

static void run_cmd(char* cmd)
{
    if (!strcmp(cmd, "uname")) {
        struct utsname utname;

        uname(&utname);
        write(1, utname.sysname, strlen(utname.sysname));
        write(1, " ", 1);
        write(1, utname.release, strlen(utname.release));
        write(1, " ", 1);
        write(1, utname.version, strlen(utname.version));
        write(1, " ", 1);
        write(1, utname.machine, strlen(utname.machine));
        write(1, " ", 1);
        write(1, utname.nodename, strlen(utname.nodename));
        write(1, "\n", 1);
        return;
    }

    write(1, cmd, strlen(cmd));
    write(1, COMMAND_NOT_FOUND, strlen(COMMAND_NOT_FOUND));
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
			run_cmd(cmd);
			write(1, SH_PREFIX, strlen(SH_PREFIX));
			idx = 0;
			tmp = cmd;
		}else{
			idx++;
			tmp++;
		}
	}
}
