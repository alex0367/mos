#ifndef FILEBLOCK_H
#define FILEBLOCK_H
#include <drivers/block.h>

block* create_fileblock();

void delete_fileblock(block* b);
#endif
