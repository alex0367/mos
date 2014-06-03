
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
LIBFLAG = -Llib -lsys
TARGETS = run sh ls # uname cat pwd mkdir rmdir reboot test
LIBS	= lib.o \
		  syscall.o

all: folder clib $(TARGETS)

clib: $(LIBS)
	$(AR) -crv libsys.a $(LIBS)
	$(LD) -shared --whole-archive -o lib/libsys.so libsys.a

folder:
	-mkdir bin
	-mkdir lib

crt0.o: crt0.s
	$(CC) $(CFLAGS) -s -o crt0.o crt0.s


sh: sh.o clib crt0.o
	$(LD) $(LDFLAGS) -o sh.dbg sh.o crt0.o libsys.a
	$(LD) $(LDFLAGS) -s -o bin/sh sh.o crt0.o libsys.a

sh.o: sh.c syscall.h
	 $(CC) $(CFLAGS) -o sh.o sh.c

run: run.o clib crt0.o
	$(LD) $(LDFLAGS) -o run.dbg run.o crt0.o libsys.a
	$(LD) $(LDFLAGS) -s -o bin/run run.o crt0.o libsys.a

run.o: run.c run.h syscall.h
	$(CC) $(CFLAGS) -o run.o run.c

uname: uname.o clib crt0.o
	$(LD) $(LDFLAGS) $(LIBFLAG) -o uname.dbg uname.o crt0.o
	$(LD) $(LDFLAGS) $(LIBFLAG) -s -o bin/uname uname.o crt0.o

uname.o: uname.c syscall.h
	$(CC) $(CFLAGS) -o uname.o uname.c

ls: ls.o clib crt0.o
	$(LD) $(LDFLAGS) -o ls.dbg ls.o crt0.o libsys.a
	$(LD) $(LDFLAGS) -s -o bin/ls ls.o crt0.o libsys.a

ls.o: ls.c syscall.h
	$(CC) $(CFLAGS) -o ls.o ls.c

cat: cat.o clib crt0.o
	$(LD) $(LDFLAGS) $(LIBFLAG) -o cat.dbg cat.o crt0.o
	$(LD) $(LDFLAGS) $(LIBFLAG) -s -o bin/cat cat.o crt0.o

cat.o: cat.c syscall.h
	$(CC) $(CFLAGS) -o cat.o cat.c

pwd: pwd.o clib crt0.o
	$(LD) $(LDFLAGS) $(LIBFLAG) -o pwd.dbg pwd.o crt0.o
	$(LD) $(LDFLAGS) $(LIBFLAG) -s -o bin/pwd pwd.o crt0.o

pwd.o: pwd.c syscall.h
	$(CC) $(CFLAGS) -o pwd.o pwd.c

mkdir: mkdir.o clib crt0.o
	$(LD) $(LDFLAGS) $(LIBFLAG) -o mkdir.dbg mkdir.o crt0.o
	$(LD) $(LDFLAGS) $(LIBFLAG) -s -o bin/mkdir mkdir.o crt0.o

mkdir.o: mkdir.c syscall.h
	$(CC) $(CFLAGS) -o mkdir.o mkdir.c


rmdir: rmdir.o clib crt0.o
	$(LD) $(LDFLAGS) $(LIBFLAG) -o rmdir.dbg rmdir.o crt0.o
	$(LD) $(LDFLAGS) $(LIBFLAG) -s -o bin/rmdir rmdir.o crt0.o

rmdir.o: rmdir.c syscall.h
	$(CC) $(CFLAGS) -o rmdir.o rmdir.c

reboot: reboot.o clib crt0.o
	$(LD) $(LDFLAGS) $(LIBFLAG) -o reboot.dbg reboot.o crt0.o
	$(LD) $(LDFLAGS) $(LIBFLAG) -s -o bin/reboot reboot.o crt0.o

reboot.o: reboot.c syscall.h
	$(CC) $(CFLAGS) -o reboot.o reboot.c



test: test.o clib crt0.o
	$(LD) $(LDFLAGS) $(LIBFLAG) -o test.dbg test.o crt0.o
	$(LD) $(LDFLAGS) $(LIBFLAG) -s -o bin/test test.o crt0.o

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
	-rm -rf lib
