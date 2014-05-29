#include <syscall.h>
#include <fs.h>

int main(int argc, char** argv)
{
    // FIXME
    struct dirent ent;
    DIR dir = 0;
	char cwd[256] = {0};

	getcwd(cwd, 256);
	dir = (DIR)opendir(cwd);
    if (!dir) {
        return -1;
    }

    while (readdir(dir, &ent)) {
        printf("%s\n", ent.d_name);
    }

    closedir(dir);

    return 0;
}
