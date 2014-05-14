#include <stdio.h>
#include <fileblock.h>
#include <fs/ffs.h>

int main (int argc, char *argv[])
{
    block* b = create_fileblock();

    ffs_attach(b);

    return(0);
}

