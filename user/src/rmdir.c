#include <syscall.h>
#include <fs.h>
#include <syscall/unistd.h>
#include <lib/klib.h>


static int rm_file(const char* f)
{
    struct stat s;
    char file[256] = {0};

    if (!f)
        return -1;

    if (f[0] != '/')
    {
        getcwd(file, 256);
        if (strcmp(file, "/") != 0)
            strcat(file, "/");
        strcat(file, f);
    }
    else
    {
        strcpy(file, f);
    }

    if (stat(file, &s) == -1)
    {
        printf("File not found\n");
        return -1;
    }

    if (!S_ISDIR(s.st_mode))
    {
        printf("Not a directory\n");
        return -1;
    }

    return rmdir(file);


}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Invalid parameter\n");
        return -1;
    }
    else
    {
        return rm_file(argv[1]);
    }
    return 0;
}
