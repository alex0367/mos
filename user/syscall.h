#ifndef _SYSCALL_H_
#define _SYSCALL_H_
#include <syscall/unistd.h>
#include <config.h>

#define SYSCALL0(no) \
	__asm__("movl %0, %%eax" : : "i"(no));\
	__asm__("int %0" : : "i"(SYSCALL_INT_NO));\

#define SYSCALL1(no, arg0) \
	__asm__("movl %0, %%ebx" : : "m"(arg0));\
	SYSCALL0(no);

#define SYSCALL2(no, arg0, arg1) \
	__asm__("movl %0, %%ecx" : : "m"(arg1));\
	SYSCALL1(no, arg0);

#define SYSCALL3(no, arg0, arg1, arg2) \
	__asm__("movl %0, %%edx" : : "m"(arg2));\
	SYSCALL2(no, arg0, arg1);

#define DEFINE_SYSCALL0(name) \
	static inline int name() \
	{ \
		SYSCALL0(__NR_##name); \
	}

#define DEFINE_SYSCALL1(name, type0, arg0) \
	static inline int name(type0 arg0)\
	{ \
		SYSCALL1(__NR_##name, arg0); \
	}

#define DEFINE_SYSCALL2(name, type0, arg0, type1, arg1) \
	static inline int name(type0 arg0, type1 arg1) \
	{ \
		SYSCALL2(__NR_##name, arg0, arg1); \
	}

#define DEFINE_SYSCALL3(name, type0, arg0, type1, arg1, type2, arg2) \
	static inline int name(type0 arg0, type1 arg1, type2 arg2) \
	{ \
		SYSCALL3(__NR_##name, arg0, arg1, arg2);\
	}

typedef void* DIR;
struct dirent;
typedef unsigned long time_t;

DEFINE_SYSCALL1(exit, unsigned, status);
DEFINE_SYSCALL0(fork)
DEFINE_SYSCALL3(read, unsigned, fd, const char*, buf, unsigned, len)
DEFINE_SYSCALL3(write, unsigned, fd, const char*, buf, unsigned, len)
DEFINE_SYSCALL1(open, const char*, path)
DEFINE_SYSCALL1(close, unsigned, fd)
DEFINE_SYSCALL3(waitpid, unsigned, pid, int*, status, int ,option);
DEFINE_SYSCALL2(creat, const char*, path, unsigned, mode)
DEFINE_SYSCALL3(execve,const char*, path, char** const, argv, char** const, env)
DEFINE_SYSCALL0(getpid);
DEFINE_SYSCALL1(uname, struct utsname*, utname);
DEFINE_SYSCALL0(sched_yield);
DEFINE_SYSCALL2(stat, const char*, path, struct stat*, buf);
DEFINE_SYSCALL1(opendir, const char*, path);
DEFINE_SYSCALL1(closedir, DIR, dir);
DEFINE_SYSCALL2(readdir, DIR, dir, struct dirent*, ent);
DEFINE_SYSCALL1(brk, unsigned, addr);
DEFINE_SYSCALL1(time, time_t*, t);
#endif
