#include <stdio.h>
#include <fileblock.h>
#include <fs/ffs.h>
#include <fs/namespace.h>

static void test_list()
{
	print_quota();
	DIR dir = fs_opendir("/");
	char name[32];
	int mode;
	while (fs_readdir(dir, name, &mode)){

		printf("%x: %s\n", mode, name);
	}
	fs_closedir(dir);
	print_quota();
}

static void test_create()
{
	print_quota();
	for (int i = 0; i < 10; i++){
		char name[32];
		sprintf(name, "/test%d", i);
		fs_create(name, S_IRWXU | S_IRWXG | S_IRWXO);
	}


	print_quota();
}

static void test_delete()
{
	print_quota();
	for (int i = 3; i < 5; i++){
		char name[32];
		sprintf(name, "/test%d", i);
		fs_delete(name);
	}


	print_quota();
}

int main (int argc, char *argv[])
{
    block* b = create_fileblock();
	char c;

	vfs_init();
	mount_init();

	printf("pleaes input cmd:\n");
	while (c = getchar()){
		if (c == 'q')
			break;

		switch (c){
		case 'f':
			ffs_format(b);
			printf("format done\n");
			break;
		case 'a':
			ffs_attach(b);
			vfs_trying_to_mount_root();
			printf("attach to file system done\n");
			break;
		case 'l':
			test_list();
			break;
		case 'c':
			test_create();
			break;
		case 'd':
			test_delete();
			break;
		case '\n':
			break;
		default:
			printf("unknow command %c\n", c);
			break;
		}
	}

	delete_fileblock(b);

    return(0);
}

