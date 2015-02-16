
ifeq ($(shell uname),Linux)
	CC              = gcc
	ASM             = nasm
	LD              = ld
	AR 				= ar
else
ifeq ($(shell uname),Darwin)
	CC              = /opt/local/bin/i386-elf-gcc
	ASM     		= /opt/local/bin/nasm
	LD              = /opt/local/bin/i386-elf-ld
	AR				= /opt/local/bin/i386-elf-ar
else
	CC				= i386-elf-gcc
	ASM				= nasm
	LD				= i386-elf-ld
	ar				= i386-elf-ar
endif
endif

CSTRICT	= -Werror=uninitialized
CFLAGS	= -ggdb3 -c -m32 $(CSTRICT) -fno-stack-protector -fno-builtin -I./ -I../
ASFLAGS	= -f elf32
LDFILE	= -m elf_i386 -T link.ld 
LDFLAGS	= $(LDFILE)
LIBFLAG = -Llib -lsys
TARGET = sh.o run.o quota.o reboot.o
#ls  uname cat pwd mkdir rmdir reboot test
#SRC = $(wildcard src/*.c)
#TARGET = $(patsubst src/%.c,%.o,$(SRC))
LIBS	= lib.o \
	syscall.o

all: folder clib $(TARGET)

%.o: src/%.c syscall.h crt0.o
	$(CC) $(CFLAGS) -o $@ $<;
	$(LD) $(LDFLAGS) -o $(subst .o,,$@).dbg $@ crt0.o libsys.a;
	$(LD) $(LDFLAGS) -s -o bin/$(subst .o,,$@) $@ crt0.o libsys.a


clib: $(LIBS)
	$(AR) -crv libsys.a $(LIBS)
	$(LD) -shared --whole-archive -o lib/libsys.so libsys.a

folder:
	-mkdir bin
	-mkdir lib

crt0.o: crt0.s
	$(CC) $(CFLAGS) -s -o crt0.o crt0.s


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
