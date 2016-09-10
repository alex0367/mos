#include <syscall.h>

int main(int argc, char** argv)
{
    if (argc == 1)
        reboot(MOS_REBOOT_CMD_RESTART);
    else if (argc == 2)
    {
        if (!strcmp(argv[1], "-p"))
            reboot(MOS_REBOOT_CMD_POWER_OFF);
    }

    return -1;
}
