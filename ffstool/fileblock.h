#ifndef FILEBLOCK_H
#define FILEBLOCK_H
#include <drivers/block.h>

block* create_fileblock(char* img);

void delete_fileblock(block* b);

void format_partition();
#endif
