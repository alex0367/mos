
ifeq ($(shell uname),Linux)
	CC              = gcc
	ASM             = nasm
	LD              = ld
else
	CC              = /opt/local/bin/i386-elf-gcc
	ASM     		= /opt/local/bin/nasm
	LD              = /opt/local/bin/i386-elf-ld
endif

CSTRICT	= -Werror=uninitialized
CFLAGS	= -g -c -m32 $(CSTRICT) -fno-stack-protector -fno-builtin -I./ -I../
ASFLAGS	= -f elf32
LDFILE	= -m elf_i386 -T link.ld 
LDFLAGS	= $(LDFILE)
TARGET	= run
OBJS	= run.o \
		  syscall.o

all: folder run sh

folder:
	-mkdir bin

sh: sh.o syscall.o
	$(LD) $(LDFLAGS) -o bin/sh sh.o syscall.o

sh.o: sh.c syscall.h
	 $(CC) $(CFLAGS) -o sh.o sh.c

run: $(OBJS)
	$(LD) $(LDFLAGS) -o bin/$(TARGET) $(OBJS)

run.o: run.c run.h
	$(CC) $(CFLAGS) -o run.o run.c

syscall.o: syscall.c syscall.h ../syscall/unistd.h
	$(CC) $(CFLAGS) -o syscall.o syscall.c

clean:
	-rm *.o
	-rm -rf bin
