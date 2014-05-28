#include <syscall.h>
#include <fs.h>
#include <syscall/unistd.h>

#define STR_NO_SUCH_FILE "No such file\n"
#define STR_IS_DIRECTORY "Is a directory\n"

static int cat_stdin()
{
	char cmd[80] = {0};
	char* tmp = cmd;
	int idx = 0;

	while(1){
		read(0, tmp, 1);
		
		if( *tmp == '\r' ){
			*tmp = '\n';
			write(1, tmp, 1);
			write(1, cmd, strlen(cmd));
			idx = 0;
			tmp = cmd;
		}else{
			write(1, tmp, 1);
			idx++;
			tmp++;
		}
	}

	return 0;
}

static int cat_file(const char* file)
{
	struct stat s;
	unsigned fd;
	unsigned size;
	char buf[64];

	if( stat(file, &s) == -1){
		write(1, STR_NO_SUCH_FILE, strlen(STR_NO_SUCH_FILE));
		return -1;
	}

	if(S_ISDIR(s.st_mode)){
		write(1, STR_IS_DIRECTORY, strlen(STR_IS_DIRECTORY));
		return -1;
	}

	fd = open(file);
	do
	{
		memset(buf, 0, 64);
		size = read(fd, buf, 64);
		if(size)
			write(1, buf, size);

	}while(size != 0);

	close(fd);

	return 1;

}

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		return cat_stdin();
	}
	else{
		return cat_file(argv[1]);
	}
    return 0;
}
