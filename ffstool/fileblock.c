#include <stdio.h>
#include <stdlib.h>
#include <fileblock.h>
#include <osdep.h>

#define BLOCK_OFFSET (1024*1024)

#define CACHE_SIZE 64
typedef struct _block_cache_item
{
	int last_time;
	int sector;
	char buf[512];
}block_cache_item;

#ifdef WIN32
#pragma pack(push)
#pragma pack(1)
#endif
struct partition_table_entry
{
	unsigned char bootable;         /* 0x00=not bootable, 0x80=bootable. */
	unsigned char start_chs[3];     /* Encoded starting cylinder, head, sector. */
	unsigned char type;             /* Partition type (see partition_type_name). */
	unsigned char end_chs[3];       /* Encoded ending cylinder, head, sector. */
	unsigned int offset;          /* Start sector offset from partition table. */
	unsigned int size;            /* Number of sectors. */
}
#ifdef WIN32
;
#pragma pack(pop)
#else
__attribute__((packed));
#endif

#ifdef WIN32
#pragma pack(push)
#pragma pack(1)
#endif
struct partition_table
{
	unsigned char loader[446];      /* Loader, in top-level partition table. */
	struct partition_table_entry partitions[4];       /* Table entries. */
	unsigned short signature;       /* Should be 0xaa55. */
}
#ifdef WIN32
;
#pragma pack(pop)
#else
	__attribute__((packed));
#endif


static block_cache_item* block_cache;

static int file_read(void* file, unsigned sector, void* buf, unsigned len);
static int file_write(void* file, unsigned sector, void* buf, unsigned len);
static int file_cached_read(void* aux, unsigned sector, void* buf, unsigned len);
static int file_cached_write(void* aux, unsigned sector, void* buf, unsigned len);
static void file_cache_flush(void* aux);

block* create_fileblock()
{
    block* ret = malloc(sizeof(*ret));
	int i = 0;

	ret->read = file_read;
	ret->write = file_write;
	block_cache = kmalloc(sizeof(block_cache_item)*CACHE_SIZE);
	for (i = 0; i < CACHE_SIZE; i++){
		memset(&block_cache[i], 0, sizeof(block_cache_item));
		block_cache[i].sector = -1;
        block_cache[i].last_time = -1;
	}

#ifdef WIN32
	ret->aux = CreateFileA("../../ffs.img", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
		NULL);
	ret->sector_size = (GetFileSize(ret->aux, NULL) - BLOCK_OFFSET) / BLOCK_SECTOR_SIZE;
	SetFilePointer(ret->aux, BLOCK_OFFSET, 0, FILE_BEGIN);
#else
	ret->aux = (void*)fopen("../ffs.img", "r+");
    fseek(ret->aux, 0, SEEK_END);
	ret->sector_size = (ftell(ret->aux) - BLOCK_OFFSET) / BLOCK_SECTOR_SIZE;
	fseek(ret->aux, 0, SEEK_SET);

#endif


    return ret;
}



void delete_fileblock(block* b)
{
	file_cache_flush(b->aux);
	kfree(block_cache);
#ifndef WIN32
    fclose(b->aux);
#else
	CloseHandle(b->aux);
#endif
	free(b);
}

static int file_cache_find(unsigned sector)
{
	int i = 0;

	for (i = 0; i < CACHE_SIZE; i++){
		if (block_cache[i].sector == sector){
			return i;
		}
	}

	return -1;
}

static int file_cache_find_oldest()
{
	int i = 0;
	int oldest = 0;
	unsigned min = 0xffffffff;

	for (i = 0; i < CACHE_SIZE; i++){
		if (block_cache[i].last_time == 0){
			return i;
		}

		if (min > block_cache[i].last_time){
			min = block_cache[i].last_time;
			oldest = i;
		}
	}

	return oldest;
}

static unsigned total_touch = 0;
static unsigned cache_hit = 0;
static unsigned cache_miss = 0;
static int file_cached_read(void* aux, unsigned sector, void* buf, unsigned len)
{
	int cache_index = file_cache_find(sector);
	total_touch++;

	if (cache_index == -1){
		cache_miss++;
		file_read(aux, sector, buf, len);
		cache_index = file_cache_find_oldest();

		if (block_cache[cache_index].sector >= 0)
			file_write(aux, block_cache[cache_index].sector, block_cache[cache_index].buf, len);

		memcpy(block_cache[cache_index].buf, buf, len);
		block_cache[cache_index].last_time = GetCurrentTime();
		block_cache[cache_index].sector = sector;
	}
	else{
		cache_hit++;
		block_cache[cache_index].last_time = GetCurrentTime();
		memcpy(buf, block_cache[cache_index].buf, len);
	}

	return len;
}

static int file_cached_write(void* aux, unsigned sector, void* buf, unsigned len)
{
	int cache_index = file_cache_find(sector);
	total_touch++;
	if (cache_index == -1){
		cache_miss++;
		cache_index = file_cache_find_oldest();

		if (block_cache[cache_index].sector >= 0)
			file_write(aux, block_cache[cache_index].sector, block_cache[cache_index].buf, len);

		memcpy(block_cache[cache_index].buf, buf, len);
		block_cache[cache_index].last_time = GetCurrentTime();
		block_cache[cache_index].sector = sector;
	}
	else{
		cache_hit++;
		block_cache[cache_index].last_time = GetCurrentTime();
		memcpy(block_cache[cache_index].buf, buf, len);
	}

    return len;
}

static void file_cache_flush(void* aux)
{
	int i = 0;

	for (i = 0; i < CACHE_SIZE; i++){
		if (block_cache[i].last_time>=0 && block_cache[i].sector >= 0){
			file_write(aux, block_cache[i].sector, block_cache[i].buf, BLOCK_SECTOR_SIZE);
		}
	}
}

static int file_read(void* aux, unsigned sector, void* buf, unsigned len)
{
#ifndef WIN32
    FILE* file = (FILE*)aux;
    unsigned len_ = (len < BLOCK_SECTOR_SIZE) ? len : BLOCK_SECTOR_SIZE;

//	printf("[file_read] sector %d\n", sector);
    fseek(file, BLOCK_OFFSET + sector * BLOCK_SECTOR_SIZE, SEEK_SET);
    fread(buf, len_, 1, file);
    return 0;
#else
	HANDLE file = (HANDLE)aux;
	unsigned len_ = (len < BLOCK_SECTOR_SIZE) ? len : BLOCK_SECTOR_SIZE;
	DWORD readed = 0;
	printf("[file_read] sector %d\n", sector);
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
//	printf("[file_write] sector %d\n", sector);
    fseek(file, BLOCK_OFFSET + sector * BLOCK_SECTOR_SIZE, SEEK_SET);
    fwrite(buf, len_, 1, file);
    return 0;
#else
	HANDLE file = (HANDLE)aux;
	unsigned len_ = (len < BLOCK_SECTOR_SIZE) ? len : BLOCK_SECTOR_SIZE;
	DWORD writed = 0;
	printf("[file_read] sector %d\n", sector);
	SetFilePointer(file, BLOCK_OFFSET + sector * BLOCK_SECTOR_SIZE, 0, FILE_BEGIN);
	WriteFile(file, buf, len, &writed, NULL);
	return writed;
#endif
}

void report_cache()
{
	if (total_touch){
		printf("total %d: hit %d, miss %d, rate %f\n", total_touch, cache_hit, cache_miss,
			(double)cache_hit / (double)total_touch);
	}
	total_touch = cache_hit = cache_miss = 0;
}

void format_partition(void* aux)
{
#ifdef WIN32
	HANDLE file = (HANDLE)aux;
	struct partition_table *pt = (struct partition_table *)kmalloc(sizeof *pt);

	DWORD readed = 0;

	SetFilePointer(file, 0, 0, FILE_BEGIN);
	ReadFile(file, pt, BLOCK_SECTOR_SIZE, &readed, NULL);
	pt->partitions[0].type = 0x21;
	SetFilePointer(file, 0, 0, FILE_BEGIN);
	WriteFile(file, pt, BLOCK_SECTOR_SIZE, &readed, NULL);
#else
    FILE* file = (FILE*)aux;
	struct partition_table *pt = (struct partition_table *)kmalloc(sizeof *pt);


	fseek(file, 0, SEEK_SET);
    fread(pt, BLOCK_SECTOR_SIZE, 1, file);
	pt->partitions[0].type = 0x21;
	fseek(file, 0, SEEK_SET);
	fwrite(pt, BLOCK_SECTOR_SIZE, 1, file);
#endif
}
