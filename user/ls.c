#include <syscall.h>
#include <fs.h>

static void print_mode(unsigned mode)
{
	char tmp[10] = {0};
	char per[3] = {'x', 'r', 'w'};
	int i = 10;

	if(S_ISDIR(mode))
	  tmp[0] = 'd';
	else
	  tmp[0] = '-';

	for (i = 9; i > 0; i--){
		if (mode & 0x00000001)
		  tmp[i] = per[i % 3];
		else
		  tmp[i] = '-';
		mode = mode >> 1;
	}
	printf("%s ", tmp);
}

static void print_links(struct stat* s)
{
	printf("1 ");
}

static void print_uname(struct stat* s)
{
	printf("root ");
}

static void print_gname(struct stat* s)
{
	printf("root ");
}

static void print_size(struct stat* s)
{
	printf("%h ", s->st_size);
}

static void print_time(struct stat* s)
{
	printf("%x ", s->st_mtime);
}

int main(int argc, char** argv)
{
    // FIXME
	// now can only list current
    struct dirent ent;
    DIR dir = 0;
	char cwd[256] = {0};

	getcwd(cwd, 256);
	dir = (DIR)opendir(cwd);
    if (!dir) {
        return -1;
    }

    while (readdir(dir, &ent)) {
		char sub[256] = {0};
		struct stat s;
		if (!strcmp(cwd, "/")){
			strcpy(sub, "/");
			strcat(sub, ent.d_name);
		}else{
			strcpy(sub, cwd);
			strcat(sub, "/");
			strcat(sub, ent.d_name);
		}
		if (stat(sub, &s) == -1){
			printf("Fatal error!\n");
			break;
		}

        print_mode(ent.d_mode);
		print_links(&s);
		print_uname(&s);
		print_gname(&s);
		print_size(&s);
		print_time(&s);
		printf("%s\n", ent.d_name);
    }

    closedir(dir);

    return 0;
}
