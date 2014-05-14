#include <stdio.h>
#include <stdlib.h>
#include <fileblock.h>

#define BLOCK_OFFSET (1024*1024)

static int file_read(void* file, unsigned sector, void* buf, unsigned len);
static int file_write(void* file, unsigned sector, void* buf, unsigned len);

block* create_fileblock()
{
    block* ret = malloc(sizeof(*ret));

    ret->read = file_read;
    ret->write = file_write;
    ret->aux = (void*)fopen("../ffs.img", "r+");

    return ret;
}

void delete_fileblock(block* b)
{
    fclose(b->aux);
}

static int file_read(void* aux, unsigned sector, void* buf, unsigned len)
{
    FILE* file = (FILE*)aux;
    unsigned len_ = (len < BLOCK_SECTOR_SIZE) ? len : BLOCK_SECTOR_SIZE;

    fseek(file, BLOCK_OFFSET + sector * BLOCK_SECTOR_SIZE, SEEK_SET);
    fread(buf, len_, 1, file);
    return 0;
}

static int file_write(void* aux, unsigned sector, void* buf, unsigned len)
{
    FILE* file = (FILE*)aux;
    unsigned len_ = (len < BLOCK_SECTOR_SIZE) ? len : BLOCK_SECTOR_SIZE;

    fseek(file, BLOCK_OFFSET + sector * BLOCK_SECTOR_SIZE, SEEK_SET);
    fwrite(buf, len_, 1, file);
    return 0;
}
