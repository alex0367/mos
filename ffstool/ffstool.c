#include <stdio.h>
#include <stdlib.h>
#include <fileblock.h>
#include <fs/ffs.h>
#include <fs/namespace.h>
#include <osdep.h>

static void test_list(char* path)
{
	DIR dir = fs_opendir(path);
	char name[32];
	int mode;
	while (fs_readdir(dir, name, &mode)){
		char sub[32];
		struct stat s;

		strcpy(sub, path);
		if (strlen(path) != 1) {
			strcat(sub, "/");
		}
		strcat(sub, name);
		fs_stat(sub, &s);
		printf("%x: %s, len %d\n", mode, name, s.st_size);
	}
	fs_closedir(dir);
}

static void test_create()
{
	int i = 0;
	fs_create("/readme.txt", S_IRWXU | S_IRWXG | S_IRWXO);
	for (i = 0; i < 10; i++){
		char name[32];
		sprintf(name, "/test%d", i);
		fs_create(name, S_IRWXU | S_IRWXG | S_IRWXO);
	}


}

static void test_read()
{
	unsigned int fd = fs_open("/readme.txt");
	char* buf = 0;
	int i = 0;

	if (fd == MAX_FD)
		return;
	printf("%d: read begin\n", GetCurrentTime());
	for (i = 0; i < (4 * 1024); i++){
		buf = malloc(1024);
		fs_read(fd, i*1024, buf, 1024);

		free(buf);
	}

	fs_close(fd);
	printf("%d: read end\n", GetCurrentTime());
}

static void test_write()
{
	unsigned int fd = fs_open("/readme.txt");
	char* buf = 0;
	int i = 0;
	printf("%d: write begin\n", GetCurrentTime());
	if (fd == MAX_FD)
		return;

	for (i = 0; i < (4 * 1024); i++){
		buf = malloc(1024);
		memset(buf, 'd', 1024);
		fs_write(fd, i*1024, buf, 1024);
		free(buf);
	}

	fs_close(fd);
	printf("%d: write end\n", GetCurrentTime());
}

static void test_delete()
{
	int i = 0;
	print_quota();
	for (i = 3; i < 5; i++){
		char name[32];
		sprintf(name, "/test%d", i);
		fs_delete(name);
	}


	print_quota();
}

static void run_cmd(block* b)
{
	char c;
	extern void report_cache();
	printf("pleaes input cmd:\n");
	while (c = getchar()){
		if (c == 'q')
			break;

		switch (c){
		case 'f':
			format_partition(b->aux);
			ffs_format(b);
			printf("format done\n");
			break;
		case 'a':
			ffs_attach(b);
			vfs_trying_to_mount_root();
			printf("attach to file system done\n");
			break;
		case 'l':
			print_quota();
			test_list("/bin");
			print_quota();
			report_cache();
			break;
		case 'c':
			print_quota();
			test_create();
			print_quota();
			break;
		case 'd':
			test_delete();
			break;
		case 'w':
			test_write();
			report_cache();
			break;
		case 'r':
			test_read();
			report_cache();
			break;
		case '\n':
			break;
		default:
			printf("unknow command %c\n", c);
			break;
		}
	}


}

static void copy_user_program(char *name, char* dst_dir)
{
	unsigned ffs_fd;
	FILE* local_f;
	unsigned len;
	char* buf;
	char ffs_name[64];
	char local_name[64];
    int _len = strlen(dst_dir);
    char _dst[64];

    strcpy(_dst, dst_dir);
	strcpy(ffs_name, dst_dir);
	strcat(ffs_name, name);
	strcpy(local_name, "../user");
    strcat(local_name, dst_dir);
	strcat(local_name, name);

    _dst[_len-1] = '\0';
	fs_create(_dst, S_IRWXU | S_IRWXG | S_IRWXO | S_IFDIR);
	fs_create(ffs_name,  S_IRWXU | S_IRWXG | S_IRWXO);
	ffs_fd = fs_open(ffs_name);
	local_f = fopen(local_name, "r");
	fseek(local_f, 0, SEEK_END);
	len = ftell(local_f);
	fseek(local_f, 0, SEEK_SET);
	buf = malloc(len);
	fread(buf, len, 1, local_f);
	fs_write(ffs_fd, 0, buf, len);
	fs_close(ffs_fd);
	fclose(local_f);

	free(buf);
}

static void user_program_callback(char* name , char* dst_dir)
{
	printf("copy %s\n", name);
	copy_user_program(name, dst_dir);
}

int main (int argc, char *argv[])
{
    block* b = create_fileblock();

	vfs_init();
	mount_init();

	if (argc == 1) {
		run_cmd(b);
	}else{
		if (!strcmp(argv[1], "format")) {
			format_partition(b->aux);
			ffs_format(b);
			ffs_attach(b);
			vfs_trying_to_mount_root();
			{
				enum_dir("../user/bin", user_program_callback, "/bin/");
                enum_dir("../user/lib", user_program_callback, "/lib/");
				enum_dir("../user/etc", user_program_callback, "/etc/");
			}
            printf("files under /bin\n");
			test_list("/bin");
            printf("files under /lib\n");
            test_list("/lib");
			printf("files under /etc\n");
            test_list("/etc");


		}

	}

	delete_fileblock(b);

    return(0);
}

