CC = gcc
CSTRICT	= -Werror=return-type -Werror=uninitialized
CFLAGS	= -m32 -arch i386 -ggdb3  -c $(CSTRICT) -fno-stack-protector -I./ -I../  -DMACOS=1 -DDEBUG

OBJS = ffs.o\
		ffstool.o\
		fileblock.o\
		osdep.o\
		vfs.o\
		namespace.o\
		mount.o\
		list.o

all: $(OBJS)
	$(CC) -m32 -arch i386 -o ffstool $(OBJS)
	-mkdir Debug
	-cp ffstool Debug/

ffs.o: ../fs/ffs.c ../fs/ffs.h
	$(CC) $(CFLAGS) -o ffs.o ../fs/ffs.c

ffstool.o: ffstool.c ../fs/ffs.h
	$(CC) $(CFLAGS) -o ffstool.o ffstool.c

fileblock.o: fileblock.c fileblock.h
	$(CC) $(CFLAGS) -o fileblock.o fileblock.c

osdep.o: osdep.c osdep.h
	$(CC) $(CFLAGS) -o osdep.o osdep.c

vfs.o: ../fs/vfs.c ../fs/vfs.h
	$(CC) $(CFLAGS) -o vfs.o ../fs/vfs.c

mount.o: ../fs/mount.c ../fs/mount.h
	$(CC) $(CFLAGS) -o mount.o ../fs/mount.c

namespace.o: ../fs/namespace.c ../fs/namespace.h
	$(CC) $(CFLAGS) -o namespace.o ../fs/namespace.c

list.o: ../lib/list.c ../lib/list.h
	$(CC) $(CFLAGS) -o list.o ../lib/list.c

clean:
	-rm *.o
	-rm ffstool
