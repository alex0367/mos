#include <syscall.h>
#include <fs.h>
#include <syscall/unistd.h>
#include <lib/klib.h>



static int mkdir_file(const char* f)
{
	struct stat s;
	char file[256] = {0};
	
	if (!f)
	  return -1;

	if (f[0] != '/'){
		getcwd(file, 256);
		if( strcmp(file, "/") != 0 )
		  strcat(file, "/");
		strcat(file, f);
	}else{
		strcpy(file, f);
	}

	if( stat(file, &s) != -1){
		printf("File already exist");
		return -1;
	}

	return mkdir(file, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);


}

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		printf("Invalid parameter\n");
		return -1;
	}
	else{
		return mkdir_file(argv[1]);
	}
    return 0;
}
