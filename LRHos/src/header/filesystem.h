#ifndef HEADER_FILESYSTEM_H
#define HEADER_FILESYSTEM_H

#include "types.h"
#include "file.h"
#include "hard_disk.h"

#define MAX_FILES_PER_PART 32  // 每个分区所支持最大创建的文件数
#define BITS_PER_SECTOR 4096   // 每扇区的位数 512*8
#define SECTOR_SIZE 512        // 扇区字节大小 512byte
#define BLOCK_SIZE SECTOR_SIZE // 块字节大小 512byte

void filesystem_init();

#endif