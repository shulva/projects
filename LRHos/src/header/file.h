#ifndef HEADER_FILE_H
#define HEADER_FILE_H

#include "types.h"
#include "stl.h"
#include "filesystem.h"

// 文件类型
enum file_types
{
    FT_UNKNOWN,  // 不支持的文件类型
    FT_FILE,     // 普通文件
    FT_DIRECTORY // 目录
};

//文件系统超级块

typedef struct super_block
{
    uint32_t magic; // 用来标识文件系统类型

    uint32_t sector_num;     // 本分区总共的扇区数
    uint32_t index_node_cnt; // 本分区中index_node数量
    uint32_t part_lba_base;  // 本分区的起始lba地址

    uint32_t block_bitmap_lba;   // 空闲块位图本身起始扇区地址
    uint32_t block_bitmap_sects; // 扇区位图本身占用的扇区数量

    uint32_t index_node_bitmap_lba;   // index_node位图起始扇区lba地址
    uint32_t index_node_bitmap_sects; // index_node位图占用的扇区数量

    uint32_t index_node_table_lba;   // index_node表起始扇区lba地址
    uint32_t index_node_table_sects; // index_node表占用的扇区数量

    uint32_t data_start_lba;     // 数据区开始的第一个扇区号
    uint32_t root_index_node_no; // 根目录所在的index_node号
    uint32_t dir_entry_size;     // 目录项大小

    uint8_t space[460];                // 加上 460 字节, 凑够 512 字节 1 扇区大小
} __attribute__((packed)) super_block; //防止编译器对齐而填充空隙

//------------------------------------------------------------------------------------------//

// index_node 结构
typedef struct index_node
{
    uint32_t index_num; // index_node 编号
    uint32_t index_size;
    uint32_t index_open_cnts; // 记录此文件被打开的次数
    _Bool write_permission;   // 写文件不能并行, 进程写文件前检查此标识

    // index_sectors[0-11]是直接块, index_sectors[13]用来存储一级间接块指针,块大小为扇区大小512bytes
    uint32_t index_sectors[13];
    double_linked_list_node index_node_tag; // 用于加入已打开的 index_node 队列
} index_node;

//----------------------------------------------------------------------------------------//

#define MAX_FILE_NAME_LEN 16 // 文件名最大长度

// 目录结构
typedef struct dir
{
    index_node *index_node;
    uint32_t dir_position;   // 记录在目录内的偏移
    uint8_t dir_buffer[512]; // 目录的数据缓冲
} dir;

// 目录项结构
typedef struct dir_entry
{
    char filename[MAX_FILE_NAME_LEN]; // 普通文件或目录名称
    uint32_t index_node;              // 普通文件或目录对应的 inode 编号
    enum file_types file_type;        // 枚举文件类型
} dir_entry;

//----------------------------------------------------------------------------------------//
#endif
