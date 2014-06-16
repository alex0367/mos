#include <fs/namespace.h>
#ifdef WIN32
#include <windows.h>
#include <osdep.h>
#elif MACOS
#include <osdep.h>
#else
#include <ps/ps.h>
#include <ps/lock.h>
#include <config.h>
#include <lib/klib.h>
#include <fs/mount.h>
#include <int/timer.h>
#include <syscall/unistd.h>
#endif

static INODE fs_lookup_inode(char* path);
static unsigned fs_get_free_fd(INODE node, char* path);
static INODE fs_get_fd(unsigned id, char* path);
static void* fs_clear_fd(unsigned fd, int* isdir);

static unsigned fs_dir_get_free_fd(DIR node, char* path);
static DIR fs_dir_get_fd(unsigned id);

unsigned fs_open(char* path)
{
    INODE node = 0;
	DIR dir = 0;
    unsigned fd;
	char* fullPath = kmalloc(64);
	memset(fullPath, 0, 64);
	

	// FIXME
	if (!strcmp(path, ".")) {
		sys_getcwd(fullPath, 64);
	}else{
		strcpy(fullPath, path);
	}

	node = fs_lookup_inode(fullPath);

	#ifdef __VERBOS_SYSCALL__
	printf("open %s, ", fullPath);
	#endif


    if (!node) {
		#ifdef __VERBOS_SYSCALL__
		printf("ret %d\n", -1);
		#endif
		kfree(fullPath);
        return 0xffffffff;
    }

	if (S_ISDIR(vfs_get_mode(node))){
		dir = vfs_open_dir(node);
		fd = fs_dir_get_free_fd(dir, fullPath);
		vfs_free_inode(node);
		if (fd == MAX_FD) {
			vfs_close_dir(dir);
		}
	}else{
		fd = fs_get_free_fd(node, fullPath);
		if (fd == MAX_FD) {
			vfs_free_inode(node);
		}
	}

	#ifdef __VERBOS_SYSCALL__
	printf("ret %d\n", fd);
	#endif
	if (fd == MAX_FD) {
		fd = 0xffffffff;
	}

	kfree(fullPath);
    return fd;
}

void fs_close(unsigned int fd)
{
    INODE node = 0;
	DIR   dir = 0;
	int isdir;
	void* ret = fs_clear_fd(fd,&isdir);

	#ifdef __VERBOS_SYSCALL__
	printf("close %d, isdir %d\n", fd, isdir);
	#endif

	if (isdir) {
		dir = ret;
		if (dir) {
			vfs_close_dir(dir);
		}
	}else{
		node = ret;
		if (node) {
			vfs_free_inode(node); 
		}
	}
}

unsigned fs_read(unsigned fd, unsigned offset, void* buf, unsigned len)
{
	char* path = kmalloc(64);
    INODE node = fs_get_fd(fd, path);
    unsigned ret;
	void* hash = 0;

	if (fd == 0xffffffff) {
		kfree(path);
		return 0xffffffff;
    }

    if (!node) {
		kfree(path);
        return 0xffffffff;
    }


	if (path && path[0]) {
		hash = file_cache_find(path);
		if (hash) {
			kfree(path);
			return file_cache_read(hash, offset, buf, len);
		}
	}

	kfree(path);
    ret = vfs_read_file(node, offset,buf,len);
    return ret;
}

unsigned fs_write(unsigned fd, unsigned offset, void* buf, unsigned len)
{
    INODE node = fs_get_fd(fd, 0);
    unsigned ret;

    if (fd == 0xffffffff) {
		return 0xffffffff;
    }

    if (!node) {
        return 0xffffffff;
    }

    ret = vfs_write_file(node, offset,buf,len);
    return ret;
}

int fs_create(char* path, unsigned mode)
{  
	DIR dir;
	char dir_name[260] = { 0 };
	char* t;
	struct stat s;
	int stat_status;

	strcpy(dir_name, path);
	t = strrchr(dir_name, '/');
	if (!t)
		return 0;

	*t = '\0';
	t++;

	if (*dir_name == '\0'){
		dir = fs_opendir("/");
		stat_status = fs_stat("/", &s);
	}
	else{
		dir = fs_opendir(dir_name);
		stat_status = fs_stat(dir_name, &s);
	}

	if (!dir)
		return 0;

	if (stat_status == -1 || !S_ISDIR(s.st_mode)){
		fs_closedir(dir);
		return 0;
	}

	if (fs_stat(path, &s) != -1){
		fs_closedir(dir);
		return 0;
	}

	vfs_add_dir_entry(dir, mode, t);


	fs_closedir(dir);
    return 1;
}

int fs_delete(char* path)
{  
	DIR dir;
	char dir_name[260] = { 0 };
	char* t;
	struct stat s;
	int stat_status;

	strcpy(dir_name, path);
	t = strrchr(dir_name, '/');
	if (!t)
		return 0;

	*t = '\0';
	t++;

	if (*dir_name == '\0'){
		dir = fs_opendir("/");
		stat_status = fs_stat("/", &s);
	}
	else{
		dir = fs_opendir(dir_name);
		stat_status = fs_stat(dir_name, &s);
	}

	if (!dir)
		return 0;

	if (stat_status == -1 || !S_ISDIR(s.st_mode)){
		fs_closedir(dir);
		return 0;
	}

	if (fs_stat(path, &s) == -1){
		fs_closedir(dir);
		return 0;
	}

	vfs_del_dir_entry(dir, t);


	fs_closedir(dir);
	return 1;
}

int fs_rename(char* path, char* new)
{  
    UNIMPL;
    return 0;
}

int fs_stat(char* path, struct stat* s)
{
    INODE node = 0;
    int ret;
	char* fullPath = kmalloc(64);
	memset(fullPath, 0, 64);
	

	// FIXME
	if (!strcmp(path, ".")) {
		sys_getcwd(fullPath, 64);
	}else{
		strcpy(fullPath, path);
	}

	#ifdef __VERBOS_SYSCALL__
	printf("stat %s, ", fullPath);
	#endif

	node = fs_lookup_inode(fullPath);
    if (!node) {
		#ifdef __VERBOS_SYSCALL__
		printf("ret %d\n", -1);
		#endif
		kfree(fullPath);
        return -1;
    }

    ret = vfs_copy_stat(node,s, 0);
    vfs_free_inode(node);
	#ifdef __VERBOS_SYSCALL__
	printf("ret %d\n", 0);
	#endif
	kfree(fullPath);
    return 0;
}

// FIXME!
// do not seperate INODE and DIR
// orelse ugly code will have to added
int fs_fstat(int fd, struct stat* s)
{
	INODE node = 0;
	DIR dir = 0;
	int ret;

	#ifdef __VERBOS_SYSCALL__
	printf("fstat %d, ", fd);
	#endif

    if (fd < 0 || fd >= MAX_FD) {
		#ifdef __VERBOS_SYSCALL__
		printf("ret %d\n", -1);
		#endif
        return -1;
    }

	node = fs_get_fd(fd, 0);

	if (!node) {
		dir = fs_dir_get_fd(fd);
	}

	if (!node && !dir) {
		#ifdef __VERBOS_SYSCALL__
		printf("ret %d\n", -1);
		#endif
        return -1;
    }

	if (node) {
		ret = vfs_copy_stat(node,s, 0);
	}else if(dir){
		ret = vfs_copy_stat(dir, s, 1);
	}
	#ifdef __VERBOS_SYSCALL__
	printf("(%x, %d)ret %d\n",s->st_mode, s->st_size, 0);
	#endif

    return 1;
}

DIR fs_opendir(char* path)
{
    INODE node = fs_lookup_inode(path);
    DIR ret;
    if (!node) {
        return 0;
    }
	if (!S_ISDIR(vfs_get_mode(node))){
		return 0;
	}

    ret = vfs_open_dir(node);
	vfs_free_inode(node);
	return ret;
}

int fs_readdir(DIR dir, char* name, unsigned* mode)
{
    INODE node = 0;

    if (!name || !mode) {
        return 0;
    }

    if (!dir) {
        *name = '\0';
        *mode = 0;
        return 0;
    }

    node = vfs_read_dir(dir);
	if (!node){
		*name = '\0';
		*mode = 0;
		return 0;
	}
	strcpy(name, vfs_get_name(node));
    *mode = vfs_get_mode(node);
    vfs_free_inode(node);
	return 1;
}

int sys_readdir(unsigned fd, struct dirent* entry)
{
	DIR dir = fs_dir_get_fd(fd);
    int ret = fs_readdir(dir, entry->d_name, &entry->d_mode);
    if (ret) {
        entry->d_namlen = strlen(entry->d_name); 
        entry->d_ino = 0;
		entry->d_reclen = ((entry->d_namlen + 10)/4 + 1) * 4;

    }

	#ifdef __VERBOS_SYSCALL__
	printf("readdir(%d, %x) = %d (%d, %d, %x, %d, %s)\n",
		   fd, entry, ret, entry->d_ino, entry->d_reclen, entry->d_mode, 
		   entry->d_namlen, entry->d_name);
	#endif

    return ret;
}

void fs_closedir(DIR dir)
{
    if (!dir) {
        return;
    }

    vfs_close_dir(dir);
}


static INODE fs_get_dirent_node(INODE node, char* name)
{
	DIR dir = 0;
	INODE entry = 0;
	int found = 0;

	if (!S_ISDIR(vfs_get_mode(node)))
	  return 0;
	
	dir = vfs_open_dir(node);
	entry = vfs_read_dir(dir);
	while(entry){
		if ( !strcmp(name, vfs_get_name(entry)) ){
			found = 1;
		  break;
		}
		
		vfs_free_inode(entry);
		entry = vfs_read_dir(dir);
	}

	vfs_close_dir(dir);
	if (found) {
		return entry;
	}else{
		return 0;
	}
}

static INODE fs_check_mountpoint(char* path)
{
	struct filesys_type* type = 0;

	type = mount_lookup(path);
	if (!type || !type->super_ops || !type->super_ops->get_root) {
		return 0;
	}


	return type->super_ops->get_root(type);

}

static INODE fs_lookup_inode(char* path)
{
    struct filesys_type* type = (void*)0;
	INODE root;
	char* parent = 0;
	char* tmp;

	if (!path || !*path)
	  return 0;
	
	// find root file system
	type = mount_lookup("/");
	if (!type)
	  return 0;
	
	root = vfs_get_root(type);
	if (!strcmp(path, "/")){
		return root;
	}
	
	parent = kmalloc(256);
	memset(parent, 0, 256);

	strcpy(parent, path);
	tmp = parent;

	// FIXME
	// check mount point every time takes too much effert
	// linux will cache dirent with mount point info attached
	//
	do{
		INODE p;
		INODE node; 
		char* slash = 0;

		tmp++;
		p = root;

		slash = strchr(tmp, '/');
		while (slash){
			*slash = '\0';
			node = fs_check_mountpoint(parent);
			if (!node) {
				node = fs_get_dirent_node(p, tmp);
			}
			vfs_free_inode(p);
			if (!node){
				*slash = '/';
				node = fs_check_mountpoint(parent);
				kfree(parent);
			  return node;
			}
			p = node;
			tmp = slash+1;
			*slash = '/';
			slash = strchr(tmp, '/');
		}
		node = fs_check_mountpoint(parent);
		if (!node) {
			node = fs_get_dirent_node(p, tmp);
		}
		vfs_free_inode(p);
		kfree(parent);
		return node;
	}while(0);
}

static unsigned fs_get_free_fd(INODE node, char* path)
{
    task_struct* cur = CURRENT_TASK();
    int i = 0;

    sema_wait(&cur->fd_lock);

    for (i = 0; i < MAX_FD; i++) {
        if (cur->fds[i].flag == 0) {
            cur->fds[i].file = node;
			cur->fds[i].file_off = 0;
			cur->fds[i].flag |= fd_flag_used;
			cur->fds[i].path = strdup(path);
            break;
        }
    }

    sema_trigger(&cur->fd_lock);

    return i;

}

static INODE fs_get_fd(unsigned fd, char* path)
{
    task_struct* cur = CURRENT_TASK();
    INODE node;

    if (fd >= MAX_FD) {
        return 0;
    }


    sema_wait(&cur->fd_lock);
	if (cur->fds[fd].flag & fd_flag_isdir) {
		node = 0;
	}else{
		node = cur->fds[fd].file;
		if (path) {
			strcpy(path, cur->fds[fd].path); 
		}
	}
    sema_trigger(&cur->fd_lock);

    return node;
}

static unsigned fs_dir_get_free_fd(DIR node, char* path)
{
    task_struct* cur = CURRENT_TASK();
    int i = 0;

    sema_wait(&cur->fd_lock);

    for (i = 0; i < MAX_FD; i++) {
        if (cur->fds[i].flag == 0) {
            cur->fds[i].dir = node;
			cur->fds[i].file_off = 0;
			cur->fds[i].flag |= fd_flag_used;
			cur->fds[i].flag |= fd_flag_isdir;
			cur->fds[i].path = strdup(path);
            break;
        }
    }

    sema_trigger(&cur->fd_lock);

    return i;
}

static DIR fs_dir_get_fd(unsigned fd)
{
    task_struct* cur = CURRENT_TASK();
    DIR node;

    if (fd >= MAX_FD) {
        return 0;
    }


    sema_wait(&cur->fd_lock);
	if (!(cur->fds[fd].flag & fd_flag_isdir)) {
		node = 0;
	}else{
		node = cur->fds[fd].dir;
	}
    sema_trigger(&cur->fd_lock);

    return node;
}

static void* fs_clear_fd(unsigned fd, int* isdir)
{
    task_struct* cur = CURRENT_TASK();
    INODE node;

    if (fd >= MAX_FD) {
        return 0;
    }

    sema_wait(&cur->fd_lock);
	*isdir = (cur->fds[fd].flag & fd_flag_isdir);
    node = cur->fds[fd].file;
    cur->fds[fd].file = 0;
	cur->fds[fd].file_off = 0;
	cur->fds[fd].flag = 0;
	kfree(cur->fds[fd].path);
	cur->fds[fd].path = 0;
    sema_trigger(&cur->fd_lock);

    return node;

}

void fs_flush(char* filesys)
{
	struct filesys_type* type = mount_lookup("/");
}

#ifdef TEST_NS
static void list_dir(char* name, int depth)
{
	char file[32];
	unsigned mode;
	DIR dir = fs_opendir(name);
	while (fs_readdir(dir, file, &mode)){
		int i = 0;
		if (!strcmp(file, ".") || !strcmp(file, "..") )
		  continue;

		for (i = 0; i < depth; i++)
		  printf("\t");
		printf("%s\n", file);
		if (S_ISDIR(mode)){
			char path[32];
			strcpy(path, name);
			if ( strcmp(name, "/") )
				strcat(path, "/");
			strcat(path, file);
			list_dir(path, depth+1);
		}
	}
	fs_closedir(dir);
}

static void test_stat(char* path)
{
	struct stat s;
	fs_stat(path, &s);
	printk("%s: is dir %d, size %d\n", path, S_ISDIR(s.st_mode), s.st_size);
}

static void test_write()
{	
	unsigned int fd = fs_open("/readme.txt");
	char* buf = 0;
	int i = 0;
	time_t now;
	unsigned t;
	timer_current(&now);
	printk("%d: write begin\n", now.seconds*60+now.milliseconds);
	if (fd == MAX_FD)
		return;

	buf = kmalloc(1024);
	memset(buf, 'd', 1024);
	t = time(0);
	for (i = 0; i < (4*1024); i++){
		if (i % 100 == 0) {
			unsigned span = time(t);
			unsigned speed = 0;
			if (span) {
				speed=((100*1024) / span) * 1000;
			}
			t = t+span;
			printk("write index %d, speed %h/s\n", i, speed);
			#ifdef DEBUG_FFS
			extern void report_time();
			extern void report_hdd_time();
			extern void report_cache();
			report_time();
			report_hdd_time();
			report_cache();
			#endif
		}
		fs_write(fd, i*1024, buf, 1024);
		
	}
	kfree(buf);
	fs_close(fd);

	timer_current(&now);
	printk("%d: write end\n", now.seconds*60+now.milliseconds);
}

static void test_read()
{	
	unsigned int fd = fs_open("/readme.txt");
	char* buf = 0;
	int i = 0;
	time_t now;
	unsigned t;
	timer_current(&now);
	printk("%d: read  begin\n", now.seconds*60+now.milliseconds);
	if (fd == MAX_FD)
		return;

	buf = kmalloc(1024);
	memset(buf, 0, 1024);
	t = time(0);
	while (1) {
		printk("");
		klogquota();
		for (i = 0; i < (4*1024); i++){
			if (i % 100 == 0) {
				unsigned span = time(0) - t;
				unsigned speed = 0;
				if (span) {
					speed=((100*1024) / span) * 1000;
				}
				t = t+span;
				printk("read index %d, speed %h/s\n", i, speed);
				#ifdef DEBUG_FFS
				extern void report_time();
				extern void report_hdd_time();
				extern void report_cache();
				report_time();
				report_hdd_time();
				report_cache();
				#endif
			}
			memset(buf, 0, 1024);
			fs_read(fd, i*1024, buf, 1024);
			
		}
	}
	kfree(buf);
	fs_close(fd);

	timer_current(&now);
	printk("%d: write end\n", now.seconds*60+now.milliseconds);
}

void test_ns()
{
	unsigned fd;
	char text[32];
	klogquota();

	printk("test_ns\n");
	//list_dir("/", 0);

	test_write();
	klogquota();
}
#endif
