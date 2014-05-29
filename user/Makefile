
ifeq ($(shell uname),Linux)
	CC              = gcc
	ASM             = nasm
	LD              = ld
	AR 				= ar
else
	CC              = /opt/local/bin/i386-elf-gcc
	ASM     		= /opt/local/bin/nasm
	LD              = /opt/local/bin/i386-elf-ld
	AR				= /opt/local/bin/i386-elf-ar
endif

CSTRICT	= -Werror=uninitialized
CFLAGS	= -ggdb3 -c -m32 $(CSTRICT) -fno-stack-protector -fno-builtin -I./ -I../
ASFLAGS	= -f elf32
LDFILE	= -m elf_i386 -T link.ld 
LDFLAGS	= $(LDFILE)
TARGETS = run sh uname ls cat pwd mkdir rmdir test
LIBS	= lib.o \
		  syscall.o

all: folder clib $(TARGETS)

clib: $(LIBS)
	$(AR) -crv libc.a $(LIBS)

folder:
	-mkdir bin

sh: sh.o clib
	$(LD) $(LDFLAGS) -o sh.dbg sh.o libc.a
	$(LD) $(LDFLAGS) -s -o bin/sh sh.o libc.a

sh.o: sh.c syscall.h
	 $(CC) $(CFLAGS) -o sh.o sh.c

run: run.o clib
	$(LD) $(LDFLAGS) -o run.dbg run.o libc.a
	$(LD) $(LDFLAGS) -s -o bin/run run.o libc.a

run.o: run.c run.h syscall.h
	$(CC) $(CFLAGS) -o run.o run.c

uname: uname.o clib
	$(LD) $(LDFLAGS) -o uname.dbg uname.o libc.a
	$(LD) $(LDFLAGS) -s -o bin/uname uname.o libc.a

uname.o: uname.c syscall.h
	$(CC) $(CFLAGS) -o uname.o uname.c

ls: ls.o clib
	$(LD) $(LDFLAGS) -o ls.dbg ls.o libc.a
	$(LD) $(LDFLAGS) -s -o bin/ls ls.o libc.a

ls.o: ls.c syscall.h
	$(CC) $(CFLAGS) -o ls.o ls.c

cat: cat.o clib
	$(LD) $(LDFLAGS) -o cat.dbg cat.o libc.a
	$(LD) $(LDFLAGS) -s -o bin/cat cat.o libc.a

cat.o: cat.c syscall.h
	$(CC) $(CFLAGS) -o cat.o cat.c

pwd: pwd.o clib
	$(LD) $(LDFLAGS) -o pwd.dbg pwd.o libc.a
	$(LD) $(LDFLAGS) -s -o bin/pwd pwd.o libc.a

pwd.o: pwd.c syscall.h
	$(CC) $(CFLAGS) -o pwd.o pwd.c

mkdir: mkdir.o clib
	$(LD) $(LDFLAGS) -o mkdir.dbg mkdir.o libc.a
	$(LD) $(LDFLAGS) -s -o bin/mkdir mkdir.o libc.a

mkdir.o: mkdir.c syscall.h
	$(CC) $(CFLAGS) -o mkdir.o mkdir.c


rmdir: rmdir.o clib
	$(LD) $(LDFLAGS) -o rmdir.dbg rmdir.o libc.a
	$(LD) $(LDFLAGS) -s -o bin/rmdir rmdir.o libc.a

rmdir.o: rmdir.c syscall.h
	$(CC) $(CFLAGS) -o rmdir.o rmdir.c



test: test.o clib
	$(LD) $(LDFLAGS) -o test.dbg test.o libc.a
	$(LD) $(LDFLAGS) -s -o bin/test test.o libc.a

test.o: test.c syscall.h
	$(CC) $(CFLAGS) -o test.o test.c


syscall.o: syscall.c syscall.h ../syscall/unistd.h
	$(CC) $(CFLAGS) -o syscall.o syscall.c

lib.o: ../lib/klib.c ../lib/klib.c
	$(CC) $(CFLAGS) -o lib.o ../lib/klib.c

clean:
	-rm *.o
	-rm *.a
	-rm *.dbg
	-rm -rf bin
