#include <config.h>
#include <ext2.h>
#include <vfs.h>
#include <block.h>
#ifdef WIN32
#include <osdep.h>
#include <time.h>
#else
#include <klib.h>
#endif
#include <unistd.h>
#include <ps.h>


static INODE ext2_create_inode(struct filesys_type*);
static void ext2_destroy_inode(struct filesys_type*, INODE);
static void ext2_write_super(struct filesys_type*, SUPORBLOCK);
static void ext2_write_desc(struct filesys_type*, DESC);
static void ext2_free_inode(struct filesys_type*, INODE);
static INODE ext2_get_root(struct filesys_type*);
static unsigned ext2_get_mode(INODE inode);
static unsigned ext2_read_file(INODE inode, unsigned int offset, void* buf, unsigned len);
static unsigned ext2_write_file(INODE inode, unsigned int offset, void* buf, unsigned len);
static DIR ext2_open_dir(INODE inode);
static INODE ext2_read_dir(DIR dir);
static void ext2_add_dir_entry(DIR dir, unsigned mode, char* name);
static void ext2_del_dir_entry(DIR dir, char* name);
static void ext2_close_dir(DIR dir);
static char* ext2_get_name(INODE node);
static unsigned int ext2_get_size(INODE node);
static int ext2_copy_stat(INODE node, struct stat* s, int is_dir);


// internal
static unsigned int ext2_get_blockno(void* type, unsigned int* blocks, unsigned int index);
static int ext2_read_block(void* type, unsigned int bno, unsigned offset, void* buf, unsigned len);
static int ext2_read_inode(void* type, unsigned int group, unsigned int ino, struct ext2_inode* buf);

#define EXT2_BLOCK_SIZE(b) \
	(b->s_log_block_size == 0 ? 1024 : \
	b->s_log_block_size == 1 ? 2048 : \
	b->s_log_block_size == 2 ? 4096 : 0)

#define EXT2_DESCS_PER_BLOCK(b) \
	(EXT2_BLOCK_SIZE(b) / sizeof(struct ext2_bg_descriptor))

#define EXT2_BNOS_PER_BLOCK(b) \
	(EXT2_BLOCK_SIZE(b) / sizeof(unsigned))

#define EXT2_INODES_PER_SECTOR(b) \
	(BLOCK_SECTOR_SIZE / b->s_inode_size)

#define EXT2_SECTORS_PER_BLOCK(b) \
	(EXT2_BLOCK_SIZE(b) / BLOCK_SECTOR_SIZE)

#define EXT2_BLOCK_GROUP_SIZE(b) \
	(8 * EXT2_BLOCK_SIZE(b) * EXT2_BLOCK_SIZE(b))


#define EXT2_BLOCK_GROUP_COUNT(dev, b) \
	((BLOCK_BYTE_SIZE(dev) - EXT2_BLOCK_SIZE(b)) / EXT2_BLOCK_GROUP_SIZE(b) + 1)


static struct super_operations ext2_super_operations = {
    ext2_create_inode,
    ext2_destroy_inode,
    ext2_write_super,
    ext2_write_desc,
    ext2_free_inode,
    ext2_get_root
};

static struct inode_opreations ext2_inode_operations = {
    ext2_get_mode,
    ext2_read_file,
    ext2_write_file,
    ext2_open_dir,
    ext2_read_dir,
    ext2_add_dir_entry,
    ext2_del_dir_entry,
    ext2_close_dir,
    ext2_get_name,
    ext2_get_size,
    ext2_copy_stat
};

#ifdef DEBUG
static void print_dir_entry(struct ext2_dir_entry_2* dir, unsigned int count)
{
    int i = 0;
    for (i = 0; i < count; i++)
    {
        char* tmp = (char*)dir;
        dir->name[dir->name_len] = '\0';
        printk("\t\ti:%d,l:%d,n:%d,t:%d\t\t%s\n",
            dir->inode, dir->rec_len, dir->name_len, dir->file_type, dir->name);

        tmp = (char*)&(dir->rec_len) + dir->rec_len - 4;;
        dir = (struct ext2_dir_entry_2*)tmp;

    }
}

#endif

void ext2_attach(block* b)
{
    struct ext2_super_block * e2sb = (struct ext2_super_block *)kmalloc(sizeof(*e2sb));
    unsigned char* buf = (unsigned char*)e2sb;
    struct ext2_bg_descriptor **bgd = 0;
    unsigned bgd_count = 0;
    unsigned i = 0;
    struct filesys_type* type;

    b->read(b->aux, 2, buf, BLOCK_SECTOR_SIZE);
    if (e2sb->s_rev_level >= 1)
    {
        buf += BLOCK_SECTOR_SIZE;
        b->read(b->aux, 3, buf, BLOCK_SECTOR_SIZE);
    }
    else
    {
        memset(b, 0, BLOCK_SECTOR_SIZE);
        e2sb->s_first_ino = EXT2_GOOD_OLD_FIRST_INO;
        e2sb->s_inode_size = EXT2_GOOD_OLD_INODE_SIZE;
    }

#ifdef DEBUG_FS

    printk("sb %x, inode_count %d, blocks_count %d\n", e2sb,
        e2sb->s_inodes_count, e2sb->s_blocks_count);
    printk("\t\tmagic %x, major %d\n", e2sb->s_magic, e2sb->s_rev_level);
    printk("\t\tfree inode %d, free blocks %d\n", e2sb->s_free_inodes_count, e2sb->s_free_blocks_count);
    printk("\t\tname %s, last mounted %s\n", e2sb->s_volume_name, e2sb->s_last_mounted);
    printk("\t\tfirst valid inode %d, first un-reserve %d\n", e2sb->s_first_data_block, e2sb->s_first_ino);
    printk("\t\tinode size %d\n", e2sb->s_inode_size);
    printk("\t\tbgcount %d, size per group %x\n", EXT2_BLOCK_GROUP_COUNT(b, e2sb), EXT2_BLOCK_GROUP_SIZE(e2sb));
    printk("\t\tblock size %d\n", EXT2_BLOCK_SIZE(e2sb));
    printk("\t\tinode per group %d\n", e2sb->s_inodes_per_group);

#endif
    if (e2sb->s_magic != 0xef53)
    {
        printk("wrong file system\n");
        kfree(e2sb);
        return;
    }


    bgd_count = EXT2_BLOCK_GROUP_COUNT(b, e2sb);
    bgd = kmalloc(bgd_count * sizeof(void*));
    memset(bgd, 0, bgd_count * sizeof(void*));

    for (i = 0; i < EXT2_BLOCK_GROUP_COUNT(b, e2sb); i++)
    {
        struct filesys_type tmpfs = {
            e2sb,
            bgd,
            b
        };
        bgd[i] = kmalloc(EXT2_BLOCK_SIZE(e2sb));
        ext2_read_block(&tmpfs, 1 + i, 0, bgd[i], EXT2_BLOCK_SIZE(e2sb));
    }

    type = register_vfs(e2sb, bgd, b, &ext2_super_operations, &ext2_inode_operations, "rootfs");
#ifdef DEBUG
    for (i = 0; i < EXT2_BLOCK_GROUP_COUNT(b, e2sb); i++)
    {
        do
        {
            printk("block group %d\n", i);
            printk("\t\tblock bitmap %d, inode bitmap %d\n", bgd[i]->bg_block_bitmap, bgd[i]->bg_inode_bitmap);
            printk("\t\tinode table location %d\n", bgd[i]->bg_inode_table);
        }
        while (0);
    }
#endif

}

static int ext2_read_inode(void* aux, unsigned int group, unsigned int ino, struct ext2_inode* buf)
{
    struct filesys_type* fs = (struct filesys_type*)aux;
    struct ext2_super_block* sb = (struct ext2_super_block*)fs->sb;
    struct ext2_bg_descriptor** bgds = (struct ext2_bg_descriptor**)fs->desc;
    struct ext2_bg_descriptor* bgd;
    block* b = (block*)fs->dev;
    int group_count = EXT2_BLOCK_GROUP_COUNT(b, sb);
    unsigned int inode_blockno = 0;
    unsigned int sec_no, sec_offset;
    void * tmp = 0;
    struct ext2_inode* tmpinode = 0;

    if (group >= group_count)
        return 0;

    bgd = bgds[group];
    inode_blockno = bgd->bg_inode_table;
    sec_no = inode_blockno * EXT2_SECTORS_PER_BLOCK(sb);
    sec_no += ((ino - 1) / EXT2_INODES_PER_SECTOR(sb));
    sec_offset = (ino - 1) % EXT2_INODES_PER_SECTOR(sb);
    tmp = CURRENT_TASK()->user.reserve;
    b->read(b->aux, sec_no, tmp, BLOCK_SECTOR_SIZE);
    tmpinode = (struct ext2_inode*)tmp;
    tmpinode += sec_offset;
    memcpy(buf, tmpinode, sizeof(*tmpinode));
    return sizeof(*buf);
}

static int ext2_read_block(void* aux, unsigned int bno, unsigned offset, void* buf, unsigned len)
{
    struct filesys_type* fs = (struct filesys_type*)aux;
    struct ext2_super_block* sb = (struct ext2_super_block*)fs->sb;
    block* b = (block*)fs->dev;
    unsigned sec_no, sec_count, i;
    char* tmp = (char*)buf;
    unsigned read_count;
    unsigned sec_off;
    sec_count = (EXT2_BLOCK_SIZE(sb) / BLOCK_SECTOR_SIZE);

    read_count = len / BLOCK_SECTOR_SIZE;
    if ((read_count*BLOCK_SECTOR_SIZE) < len)
        read_count++;

    sec_no = bno * sec_count;
    sec_off = offset / BLOCK_SECTOR_SIZE;
    sec_no += sec_off;
    for (i = 0; i < read_count; i++)
    {
        b->read(b->aux, sec_no + i, tmp, BLOCK_SECTOR_SIZE);
        tmp += BLOCK_SECTOR_SIZE;
    }

    return (sec_no*BLOCK_SECTOR_SIZE);
}


static INODE ext2_create_inode(struct filesys_type* type)
{
    UNIMPL;
    return 0;
}

static void ext2_destroy_inode(struct filesys_type* type, INODE node)
{
    UNIMPL;
}

static void ext2_write_super(struct filesys_type* type, SUPORBLOCK sb)
{
    UNIMPL;
}

static void ext2_write_desc(struct filesys_type* type, DESC desc)
{
    UNIMPL;
}
static void ext2_free_inode(struct filesys_type* type, INODE node)
{
    struct ext2_inode_info* info = (struct ext2_inode_info*)node;
    kfree(info->inode);
    kfree(node);
}

static INODE ext2_get_root(struct filesys_type* type)
{
    struct ext2_inode* node = kmalloc(sizeof(*node));
    struct ext2_inode_info* info = kmalloc(sizeof(*info));

    info->inode = node;
    info->inode_no = EXT2_ROOT_INO;
    ext2_read_inode(type, 0, EXT2_ROOT_INO, node);
    strcpy(info->name, "");
    info->type = type;
    info->ref_count = 0;
    return info;

}


static unsigned ext2_get_mode(INODE node)
{
    struct ext2_inode_info* info = (struct ext2_inode_info*)node;
    struct ext2_inode* inode = (struct ext2_inode*)info->inode;
    return (info->inode->i_mode);
}

static unsigned ext2_read_file(INODE node, unsigned int offset, void* buf, unsigned len)
{
    struct ext2_inode_info* info = (struct ext2_inode_info*)node;
    struct ext2_inode* inode = (struct ext2_inode*)info->inode;
    struct ext2_super_block* sb = (struct ext2_super_block*)info->type->sb;
    unsigned int block_size = EXT2_BLOCK_SIZE(sb);
    unsigned int blockno = offset / block_size;
    unsigned int blockoff = offset % block_size;
    unsigned int remain;
    char* tmpbuf = buf;
    char* block_buf = CURRENT_TASK()->user.reserve;

    if ((offset + len) > inode->i_size)
    {
        len = inode->i_size - offset;
    }

    remain = len;
    while (remain)
    {
        char* tmp;
        unsigned int read_len = ((blockoff + remain) > block_size) ?
            (block_size - blockoff) :
            remain;
        unsigned data_block = -1;
        

        data_block = ext2_get_blockno(info->type, inode->i_block, blockno);
        if (data_block == 0)
            break;

        if ((blockoff == 0) && (read_len == block_size))
        {
            ext2_read_block(info->type, data_block, 0, tmpbuf, read_len);
        }
        else if (blockoff != 0)
        {
            unsigned sec_off = blockoff / BLOCK_SECTOR_SIZE * BLOCK_SECTOR_SIZE;
            unsigned sec_remain = blockoff - sec_off;
            ext2_read_block(info->type, data_block, blockoff, block_buf, read_len);
            memcpy(tmpbuf, block_buf+sec_remain, read_len);
        }
        else
        {
            ext2_read_block(info->type, data_block, 0, block_buf, read_len);
            memcpy(tmpbuf, block_buf, read_len);
        }



        remain -= read_len;
        if (!remain)
            break;

        blockno++;
        blockoff = 0;
        tmpbuf += read_len;
    }

    //kfree(block_buf);
    return (len - remain);

}
static unsigned ext2_write_file(INODE inode, unsigned int offset, void* buf, unsigned len)
{
    UNIMPL;
    return 0;
}

static DIR ext2_open_dir(INODE node)
{
    struct ext2_dir_info* dir = kmalloc(sizeof(*dir));
    struct ext2_inode_info* info = (struct ext2_inode_info*)node;
    struct ext2_super_block* sb = 0;
    unsigned int data_block = 0;

    sb = (struct ext2_super_block*)info->type->sb;


    strcpy(dir->name, info->name);
    dir->cur = 0;
    dir->inode = kmalloc(sizeof(*dir->inode));
    memcpy(dir->inode, info->inode, sizeof(*dir->inode));
    dir->type = info->type;
    dir->max = 0xFF;
    if (dir->max == 0)
    {
        kfree(dir);
        return 0;
    }
    dir->cur_block_idx = 0;
    data_block = dir->inode->i_block[dir->cur_block_idx];

    if (!data_block)
    {
        kfree(dir);
        return 0;
    }
    dir->cur_block = kmalloc(EXT2_BLOCK_SIZE(sb));
    ext2_read_block(dir->type, data_block, 0, dir->cur_block, EXT2_BLOCK_SIZE(sb));
    dir->dir = (struct ext2_dir_entry_2*)dir->cur_block;
    dir->ref_count = 0;
    return dir;
}

static INODE ext2_read_dir(DIR d)
{
    struct ext2_dir_info* dir = (struct ext2_dir_info*)d;
    struct ext2_dir_entry_2* entry = dir->dir;
    struct ext2_super_block* sb = (struct ext2_super_block*)dir->type->sb;
    int i = 0;
    struct ext2_inode_info* ret = 0;

    if (!entry)
    {
        return 0;
    }

    // return current and move to next
    ret = kmalloc(sizeof(*ret));
    ret->inode = kmalloc(sizeof(*ret->inode));
    ret->inode_no = entry->inode;
    ext2_read_inode(dir->type, 0, entry->inode, ret->inode);
    ret->type = dir->type;
    memcpy(ret->name, entry->name, entry->name_len);
    ret->name[entry->name_len] = '\0';

    dir->cur++;
    if (dir->cur >= dir->max)
    {
        dir->dir = 0;
    }
    else
    {
        char* tmp = (char*)&(entry->rec_len) + entry->rec_len - OFFSET_OF(struct ext2_dir_entry_2, rec_len);
        if (tmp >= ((char*)dir->cur_block + EXT2_BLOCK_SIZE(sb)))
        {
            unsigned data_block;
            dir->cur_block_idx++;
            data_block = ext2_get_blockno(dir->type, dir->inode->i_block, dir->cur_block_idx);
            if (!data_block)
            {
                dir->dir = 0;
            }
            else
            {
                ext2_read_block(dir->type, data_block, 0, dir->cur_block, EXT2_BLOCK_SIZE(sb));
                dir->dir = (struct ext2_dir_entry_2*)dir->cur_block;
            }
        }
        else
        {
            dir->dir = (struct ext2_dir_entry_2*)tmp;
        }
    }



    ret->ref_count = 0;
    return ret;
}

static void ext2_add_dir_entry(DIR dir, unsigned mode, char* name)
{
    UNIMPL;
}

static void ext2_del_dir_entry(DIR dir, char* name)
{
    UNIMPL;
}

static void ext2_close_dir(DIR d)
{
    struct ext2_dir_info* dir = (struct ext2_dir_info*)d;
    kfree(dir->inode);
    kfree(dir->cur_block);
    kfree(dir);
}

static char* ext2_get_name(INODE node)
{
    struct ext2_inode_info* info = (struct ext2_inode_info*)node;
    return info->name;
}

static unsigned int ext2_get_size(INODE node)
{
    struct ext2_inode_info* info = (struct ext2_inode_info*)node;
    return info->inode->i_size;
}

static int ext2_copy_stat(INODE node, struct stat* s, int is_dir)
{
    struct ext2_inode_info* info = (struct ext2_inode_info*)node;
    struct ext2_inode* inode = (struct ext2_inode*)info->inode;
    struct ext2_super_block* sb = (struct ext2_super_block*)info->type->sb;

    s->st_atime = inode->i_atime;
    //s->st_blksize = EXT2_BLOCK_SIZE(sb);
    //s->st_blocks = inode->i_blocks;
    s->st_ctime = inode->i_ctime;
    s->st_dev = (unsigned)info->type;
    s->st_gid = inode->i_gid;
    s->st_ino = info->inode_no;
    s->st_mode = inode->i_mode;
    s->st_mtime = inode->i_mtime;
    s->st_nlink = inode->i_links_count;
    s->st_size = inode->i_size;
    s->st_uid = inode->i_uid;

    return 1;
}




static unsigned int ext2_get_blockno(void* aux, unsigned int* blocks, unsigned int index)
{
    struct filesys_type* fs = (struct filesys_type*)aux;
    struct ext2_super_block* sb = (struct ext2_super_block*)fs->sb;
    unsigned int block_size = EXT2_BLOCK_SIZE(sb);

    if (index < EXT2_NDIR_BLOCKS)
    {
        return blocks[index];
    }
    else if ((index >= EXT2_NDIR_BLOCKS) && (index < (EXT2_NDIR_BLOCKS + EXT2_BNOS_PER_BLOCK(sb))))
    {
        // from 12 to (BLOCK_SIZE / 4) + 12, in indirect table
        unsigned indirect_node = blocks[EXT2_NDIR_BLOCKS];
        unsigned int* indirect_table = (unsigned int*)kmalloc(block_size);
        unsigned ret = 0;

        ext2_read_block(aux, indirect_node, 0, indirect_table, block_size);
        ret = indirect_table[index - EXT2_NDIR_BLOCKS];
        //printk("unimpl indirect block table, block index %d, file index %x\n", index, index*block_size);
        kfree(indirect_table);
        return ret;
    }
    else
    {
        printk("unimpl double indirect and trible indirect block table, block index %d, file index %x\n", index, index*block_size);
        return 0;
    }
}


