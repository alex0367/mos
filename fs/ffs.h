/*
 * fool's file system with fool's feature: 
 *      1. 32 byte file name
 *      2. 64  byte meta data for each file
 *          * 4  byte length
 *          * 4  byte mode
 *          * 4  byte create time
 *          * 4  byte modified time
 *          * 4  byte access time
 *          * 12 byte data sector number array
 *                  > for a directory, more then 128K sub files
 *                  > for a file, around 8M
 *                  > for a link, only array[0] available
 *          * 32 byte name
 *      3. first sector is super block, which contains
 *          * total size
 *          * used size
 *          * sector number for first bitmap
 *          * meta data for root dir
 *          * reserved
 *  
 */

 #ifndef _FFS_H_
 #define _FFS_H_

 #include <fs/vfs.h>
 #include <drivers/block.h>

#define FFS_MAGIC 0xffee0011

struct ffs_meta_info
{
    unsigned len; // for a dir it's sub-file count, for a file it's file size
    unsigned mode;
    unsigned mt_create;
    unsigned mt_modify;
    unsigned mt_access;
    unsigned sectors[3];
    char name[32];
};
 
struct ffs_super_node
{
    unsigned total_size;
    unsigned used_size;
    unsigned bitmap_sector;
    unsigned magic;
    struct ffs_meta_info root;
    char reserve[416];
};

struct ffs_inode
{
    struct filesys_type* type;
    struct ffs_meta_info meta;
};

struct ffs_dir
{
    struct filesys_type* type;
    unsigned cur;
    struct ffs_meta_info meta;
};

void ffs_attach(block* b);

void ffs_format(block* b);

 #endif