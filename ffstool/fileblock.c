#include <stdio.h>
#include <stdlib.h>
#include <fileblock.h>
#include <osdep.h>

#define BLOCK_OFFSET (1024*1024)

static int file_read(void* file, unsigned sector, void* buf, unsigned len);
static int file_write(void* file, unsigned sector, void* buf, unsigned len);

block* create_fileblock()
{
    block* ret = malloc(sizeof(*ret));

    ret->read = file_read;
    ret->write = file_write;
#ifdef WIN32
	ret->aux = CreateFileA("../../ffs.img", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
		NULL);
	ret->sector_size = (GetFileSize(ret->aux, NULL) - BLOCK_OFFSET) / BLOCK_SECTOR_SIZE;
	SetFilePointer(ret->aux, BLOCK_OFFSET, 0, FILE_BEGIN);
#else
	ret->aux = (void*)fopen("../ffs.img", "r+");
	ret->sector_size = (ftell(ret->aux) - BLOCK_OFFSET) / BLOCK_SECTOR_SIZE;
	fseek(ret->aux, 0, SEEK_SET);

#endif


    return ret;
}

void delete_fileblock(block* b)
{
#ifndef WIN32
    fclose(b->aux);
#else
	CloseHandle(b->aux);
#endif
	free(b);
}

static int file_read(void* aux, unsigned sector, void* buf, unsigned len)
{
#ifndef WIN32
    FILE* file = (FILE*)aux;
    unsigned len_ = (len < BLOCK_SECTOR_SIZE) ? len : BLOCK_SECTOR_SIZE;

	//printk("[file_read] sector %d\n", sector);
    fseek(file, BLOCK_OFFSET + sector * BLOCK_SECTOR_SIZE, SEEK_SET);
    fread(buf, len_, 1, file);
    return 0;
#else
	HANDLE file = (HANDLE)aux;
	unsigned len_ = (len < BLOCK_SECTOR_SIZE) ? len : BLOCK_SECTOR_SIZE;
	DWORD readed = 0;
	//printk("[file_read] sector %d\n", sector);
	SetFilePointer(file, BLOCK_OFFSET + sector * BLOCK_SECTOR_SIZE, 0, FILE_BEGIN);
	ReadFile(file, buf, len, &readed, NULL);
	return readed;
#endif

}

static int file_write(void* aux, unsigned sector, void* buf, unsigned len)
{
#ifndef WIN32
    FILE* file = (FILE*)aux;
    unsigned len_ = (len < BLOCK_SECTOR_SIZE) ? len : BLOCK_SECTOR_SIZE;
	//printk("[file_write] sector %d\n", sector);
    fseek(file, BLOCK_OFFSET + sector * BLOCK_SECTOR_SIZE, SEEK_SET);
    fwrite(buf, len_, 1, file);
    return 0;
#else
	HANDLE file = (HANDLE)aux;
	unsigned len_ = (len < BLOCK_SECTOR_SIZE) ? len : BLOCK_SECTOR_SIZE;
	DWORD writed = 0;
	//printk("[file_read] sector %d\n", sector);
	SetFilePointer(file, BLOCK_OFFSET + sector * BLOCK_SECTOR_SIZE, 0, FILE_BEGIN);
	WriteFile(file, buf, len, &writed, NULL);
	return writed;
#endif
}
