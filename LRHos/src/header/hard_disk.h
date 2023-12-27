#ifndef HEADER_HARD_DISK_H
#define HEADER_HARD_DISK_H

#include "types.h"
#include "thread.h"
#include "sync.h"
#include "timer.h"
#include "port.h"
#include "string.h"
#include "heap.h"
#include "file.h"

typedef struct disk_part disk_part;
typedef struct disk disk;
typedef struct disk_channel disk_channel;

//分区结构
struct disk_part
{
    uint32_t start_LBA;  // 起始扇区 logical block address
    uint32_t sector_num; //扇区数

    disk *mydisk;                     //分区所属的硬盘
    double_linked_list_node part_tag; //队列中标记

    char name[8];
    super_block *super_block;

    bitmap block_bitmap;            //块位图
    bitmap inode_bitmap;            // inode位图
    double_linked_list open_inodes; //此分区打开的inode队列
};

//硬盘结构
struct disk
{
    char name[8];              //硬盘名称
    disk_channel *mychannel;   //此块硬盘是哪个channel的
    uint8_t dev_num;           //主盘从盘?
    disk_part primary_part[4]; // 4个主分区
    disk_part logical_part[8]; //支持8个逻辑分区
};

// ata通道结构
struct disk_channel
{
    char name[8];
    uint16_t port;         //通道端口号
    uint8_t interrupt_num; //通道中断号
    lock lock;
    _Bool interrupt_expect; //等待硬盘的中断
    semaphore semaphore;    //阻塞或唤醒驱动程序
    disk device[2];         //主盘从盘
};

uint8_t disk_channel_num; //按硬盘数计算的通道数
disk_channel channels[2]; //有两个ide通道

//硬盘初始化
void disk_init();
//选择读写的硬盘
static void select_disk(disk *hard_disk);
//向硬盘写入起始扇区地址以及要读写的扇区数
static void select_sector(disk *hard_disk, uint32_t lba, uint8_t sector_num);
//硬盘读入sector_num个扇区的数据到buffer
static void read_from_sector(disk *hard_disk, void *buffer, uint8_t sector_num);
//将Buffer中扇区的数据写入硬盘
static void write_to_sector(disk *hard_disk, void *buffer, uint8_t sector_num);
// wait 30s..
static _Bool busy_wait(disk *hard_disk);
//从硬盘读取sector_num个扇区到Buffer
void disk_read();
//硬盘中断处理程序
void interrupt_hard_disk_handler();
//硬盘读取 read sector_num sector from disk to buffer
void disk_read(disk *hard_disk, uint32_t lba, void *buffer, uint32_t sector_num);
//硬盘写入 read sector_num sector from buffer to disk
void disk_write(disk *hard_disk, uint32_t lba, void *buffer, uint32_t sector_num);

//////////////////////////////////////////////////////////////////////////

/* 用于记录总扩展分区的起始 lba, 初始为 0， part_scan 时以此为标记 */
static int32_t ext_lba_base = 0;

// 用来记录硬盘主分区和逻辑分区的下标
static uint8_t primary_part_num = 0;
static uint8_t logical_part_num = 0;

// 分区list
double_linked_list part_list;

/* 构建1个16字节大小的结构体, 用来存分区表项 */
typedef struct part
{
    uint8_t boot_able;      // 是否可引导
    uint8_t start_head;     // 起始磁头号
    uint8_t start_sector;   // 起始扇区号
    uint8_t start_cylinder; // 起始柱面号

    uint8_t part_type;    // 分区类型
    uint8_t end_head;     // 结束磁头号
    uint8_t end_sector;   // 结束扇区号
    uint8_t end_cylinder; // 结束柱面号

    uint32_t start_lba;  // 本分区起始扇区的 lba
    uint32_t sector_num; // 本分区的扇区数目
} __attribute__((packed)) part;

/* 引导扇区,mbr或ebr所在的扇区 */
typedef struct boot_sector
{
    uint8_t mbr[446];   //主引导记录
    part part_table[4]; // 4个16字节的“磁盘分区表”（DPT）
    uint16_t signature; //启动扇区的结束标志是 0x55 0xaa

} __attribute__((packed)) boot_sector;

// 获得硬盘参数信息
static void identify_disk(disk *hard_disk);
// 扫描硬盘中地址为 ext_lba 的扇区中的所有分区
static void partition_scan(disk *hard_disk, uint32_t ext_lba);
//打印分区信息
static _Bool partition_info(double_linked_list_node *pelem, int unused);

#endif