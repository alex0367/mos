#ifndef _KLIB_H_
#define _KLIB_H_
#include <mm/mm.h>

void klib_init();

void klib_putchar(char c);

void klib_putint(int num);

void klib_print(char *str);

void klib_info(char *info, int num, char* end);

void klib_clear();

char* klib_itoa(char* str, int num);

// following is above 0xC0000000 code & data
// reserve one page to setup page table itself
#define KHEAP_END	 0xC07FF000
#define KHEAP_BEGIN	 0xC0300000
#define NULL (void*)0
typedef struct _kblock
{
	unsigned int size;
	struct _kblock *next;
}kblock;

typedef char * va_list;

#define _INTSIZEOF(n) ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )

#define va_start(ap,v) ( ap = (va_list)&v + _INTSIZEOF(v) )

#define va_arg(ap,t) ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )

#define va_end(ap) ( ap = (va_list)0 )

void* kmalloc(unsigned int size);

void kfree(void* buf);

void memcpy(void* src, void* dst, unsigned len);

void memmove(void* src, void* dst, unsigned len);

int memcmp(void* src, void* dst, unsigned len);

void memset(void* src, char val, int len);

int isspace(const char c);
 
unsigned strlen(const char* str); 

char* strcpy(char* dst, const char* src);

char* strstr(const char* src, const char* str);

char* strrev(char *src);

int strcmp(char* str, char* dst);

char* strcat(char* str, char* msg);

char* strchr(char* str, char c);

void printf(const char* str, ...);

void printk(const char* str, ...);

void vprintf(const char* str, va_list ap);

void print_human_readable_size (unsigned int size) ;

char* itoa(int num, int base, int sign);


// misc
//
void shutdown();
#endif