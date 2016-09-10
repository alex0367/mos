#include <syscall.h>


int main()
{
    struct utsname name;
    uname(&name);
    write(1, name.sysname, strlen(name.sysname));
    write(1, " ", 1);
    write(1, name.version, strlen(name.version));
    write(1, " ", 1);
    write(1, name.release, strlen(name.release));
    write(1, " ", 1);
    write(1, name.machine, strlen(name.machine));
    write(1, " ", 1);
    write(1, name.nodename, strlen(name.nodename));
    write(1, "\n", 1);
    return 0;
}


