#include <syscall.h>
#include <fs.h>

int main(int argc, char** argv)
{
    // FIXME
    struct dirent ent;
    DIR dir = (DIR)opendir("/bin");
    if (!dir) {
        return -1;
    }

    while (readdir(dir, &ent)) {
        write(1, ent.d_name, ent.d_namlen);
        write(1, "\n", 1);
    }

    closedir(dir);

    return 0;
}
