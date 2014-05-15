#include <stdio.h>
#include <fileblock.h>
#include <fs/ffs.h>

int main (int argc, char *argv[])
{
    block* b = create_fileblock();
	char c;


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
			printf("attach to file system done\n");
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

