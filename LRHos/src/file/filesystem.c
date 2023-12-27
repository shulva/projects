#include "../header/filesystem.h"

disk_part *cur_part; // 默认情况下操作的是哪个分区

// 在分区链表中找到名为 part_name 的分区, 并将其指针赋值给 cur_part
static _Bool mount_partition(struct list_elem *pelem, int arg)
{
    char *part_name = (char *)arg;
    disk_part *part = element_entry(disk_part, part_tag, pelem);

    if (!strcmp(part->name, part_name))
    {
        cur_part = part;
        disk *hard_disk = cur_part->mydisk;

        super_block *super_block_buf = (super_block *)kmalloc(SECTOR_SIZE);  // super_block_buf 用来存储从硬盘上读入的超级块
        cur_part->super_block = (super_block *)kmalloc(sizeof(super_block)); // 在内存中创建分区 cur_part 的超级块

        if (cur_part->super_block == NULL)
        {
            PANIC("alloc memory failed!");
        }

        // 读入超级块
        memset(super_block_buf, 0, SECTOR_SIZE);
        disk_read(hard_disk, cur_part->start_LBA + 1, super_block_buf, 1);
        memcpy(cur_part->super_block, super_block_buf, sizeof(super_block));                                       // 把 super_block_buf 中超级块的信息复制到分区的超级块 super_block 中
        cur_part->block_bitmap.bits = (uint8_t *)kmalloc((super_block_buf->block_bitmap_sects + 1) * SECTOR_SIZE); // 将硬盘上的块位图读入到内存

        if (cur_part->block_bitmap.bits == NULL)
        {
            PANIC("alloc memory failed!");
        }

        cur_part->block_bitmap.bitmap_bytes_len = (super_block_buf->block_bitmap_sects + 1) * SECTOR_SIZE; // 从硬盘上读入块位图到分区的 block_bitmap.bits
        disk_read(hard_disk, super_block_buf->block_bitmap_lba, cur_part->block_bitmap.bits, super_block_buf->block_bitmap_sects);

        // 将硬盘上的 inode 位图读入到内存
        cur_part->inode_bitmap.bits = (uint8_t *)kmalloc((super_block_buf->block_bitmap_sects + 1) * SECTOR_SIZE);
        if (cur_part->inode_bitmap.bits == NULL)
        {
            PANIC("alloc memory failed!");
        }
        cur_part->inode_bitmap.bitmap_bytes_len = super_block_buf->index_node_bitmap_sects * SECTOR_SIZE;

        for (int i = 0; i < SECTOR_SIZE; i++)
        {
            cur_part->inode_bitmap.bits[i] = cur_part->block_bitmap.bits[i + SECTOR_SIZE * super_block_buf->block_bitmap_sects];
        }

        // 从硬盘上读入 inode 位图到分区的 inode_bitmap.bits
        // disk_read(hard_disk, super_block_buf->index_node_bitmap_lba, cur_part->inode_bitmap.bits, super_block_buf->index_node_bitmap_sects);

        list_init(&cur_part->open_inodes);

        info;
        console_write("mount");
        console_write(part->name);

        return TRUE; // 使 list_traversal 停止遍历
    }
    return FALSE; // 使 list_traversal 继续遍历
}

static void partition_format(disk_part *part)
{
    // 为方便实现, 一个块大小是一扇区
    uint32_t boot_sector_sects = 1;
    uint32_t super_block_sects = 1;

    // index_node结点位图占用的扇区数, 最多支持 4096 个文件
    // index_node 位图占用的扇区数
    uint32_t index_node_bitmap_sects = DIV_ROUND_UP(MAX_FILES_PER_PART, BITS_PER_SECTOR);

    // index_node_table 数组占用的扇区数
    uint32_t index_node_table_sects = DIV_ROUND_UP(((sizeof(index_node) * MAX_FILES_PER_PART)), SECTOR_SIZE);

    uint32_t used_sects = boot_sector_sects + super_block_sects + index_node_bitmap_sects + index_node_table_sects; // 1+1+1+608

    // 分区总扇区 - 使用的扇区 = 可用的扇区s
    uint32_t free_sects = part->sector_num - used_sects;

    // 简单处理块位图占据的扇区数
    uint32_t block_bitmap_sects;
    block_bitmap_sects = DIV_ROUND_UP(free_sects, BITS_PER_SECTOR);
    // block_bitmap_bit_len 位图中位的长度, 可用块的数量
    uint32_t block_bitmap_bit_len = free_sects - block_bitmap_sects;
    block_bitmap_sects = DIV_ROUND_UP(block_bitmap_bit_len, BITS_PER_SECTOR);

    // 超级块初始化
    super_block super_block;

    super_block.magic = 0x12348765;
    super_block.sector_num = part->sector_num;
    super_block.index_node_cnt = MAX_FILES_PER_PART;
    super_block.part_lba_base = part->start_LBA;

    // 第 0 块是引导块, 第 1 块是超级块
    super_block.block_bitmap_lba = super_block.part_lba_base + 2;
    super_block.block_bitmap_sects = block_bitmap_sects;

    super_block.index_node_bitmap_lba = super_block.block_bitmap_lba + super_block.block_bitmap_sects;
    super_block.index_node_bitmap_sects = index_node_bitmap_sects;

    super_block.index_node_table_lba = super_block.index_node_bitmap_lba + super_block.index_node_bitmap_sects;
    super_block.index_node_table_sects = index_node_table_sects;

    super_block.data_start_lba = super_block.index_node_table_lba + super_block.index_node_table_sects;

    // 根目录的index_node编号是 0
    super_block.root_index_node_no = 0;
    super_block.dir_entry_size = sizeof(dir_entry);

    //----------------------------------------------------------//
    disk *hard_disk = part->mydisk;

    //将超级块写入本分区的01扇区

    disk_write(hard_disk, part->start_LBA + 1, &super_block, 1);

    // 找出数据量最大的元信息, 用其尺寸做存储缓冲区
    uint32_t buf_size = (super_block.block_bitmap_sects >= super_block.index_node_bitmap_sects ? super_block.block_bitmap_sects : super_block.index_node_bitmap_sects);
    buf_size = (buf_size >= super_block.index_node_table_sects ? buf_size : super_block.index_node_table_sects) * SECTOR_SIZE;

    // console_write_dec(super_block.block_bitmap_sects, black, white);8
    // console_write(" ");
    // console_write_dec(super_block.index_node_bitmap_sects, black, white);1
    // console_write(" ");
    // console_write_dec(super_block.index_node_table_sects, black, white);3

    // console_write_dec(super_block.block_bitmap_lba, black, white);65
    // console_write(" ");
    // console_write_dec(super_block.index_node_bitmap_lba, black, white);73
    // console_write(" ");
    // console_write_dec(super_block.index_node_table_lba, black, white);74

    uint8_t *buffer = (uint8_t *)kmalloc(buf_size);
    // 将空闲块位图初始化并写入 super_block.block_bitmap_lba
    // 初始化空闲块位图 block_bitmap

    buffer[0] |= 0x01; // 第 1 个块预留给根目录, 位图中先占位
    uint32_t block_bitmap_last_byte = block_bitmap_bit_len / 8;
    uint8_t block_bitmap_last_bit = block_bitmap_bit_len % 8;

    uint32_t last_size = SECTOR_SIZE - (block_bitmap_last_byte % SECTOR_SIZE); // last_size是位图所在最后一个扇区中, 不足一扇区的其余部分

    // console_write_dec(block_bitmap_last_byte, black, white);
    // console_write(" ");
    // console_write_dec(last_size, black, white);
    // console_write(" ");

    memset(&buffer[block_bitmap_last_byte], 0xff, last_size); // 1将位图最后一字节到其所在的扇区的结束全置为 1,即超出实际块数的部分直接置为已占用
    uint8_t bit_idx = 0;                                      // 2 再将上一步中覆盖的最后一字节内的有效位重新置 0

    while (bit_idx <= block_bitmap_last_bit)
    {
        buffer[block_bitmap_last_byte] &= ~(1 << bit_idx++);
    }

    disk_write(hard_disk, super_block.block_bitmap_lba, buffer, super_block.block_bitmap_sects);

    //---------------------------------------------------------//
    //下面的memset会导致相关硬盘读写出错，不知道为什么。。。
    // memset(buffer, 0, buf_size); //将inode位图初始化并写入 super_block.inode_bitmap_lba

    buffer[0] |= 0x1; // 第 0 个 inode给了根目录

    disk_write(hard_disk, super_block.index_node_bitmap_lba, buffer, super_block.index_node_bitmap_sects);

    // memset(buffer, 0, buf_size); //将inode数组初始化并写入 super_block.inode_table_lba

    buffer[0] |= 0x0; //只好手动消除影响了

    index_node *inode = (index_node *)buffer;           // 准备写 inode_table 中的第 0 项, 即根目录所在的 inode
    inode->index_size = super_block.dir_entry_size * 2; // . 和 ..的大小之和
    inode->index_num = 0;                               // 根目录占 inode 数组中第 0 个 inode，将其安排在最开始的空闲块中
    inode->index_sectors[0] = super_block.data_start_lba;

    disk_write(hard_disk, super_block.index_node_table_lba, buffer, super_block.index_node_table_sects);

    // memset(buffer, 0, buf_size);
    dir_entry *dir = (dir_entry *)buffer;

    memcpy(dir->filename, ".", 1); // 写入根目录的两个目录项 . 和 ..  初始化当前目录 .

    dir->index_node = 0;
    dir->file_type = FT_DIRECTORY;

    dir++;

    memcpy(dir->filename, "..", 2); // 初始化当前目录父目录 ..
    dir->index_node = 0;            // 根目录的父目录依然是根目录自己
    dir->file_type = FT_DIRECTORY;

    disk_write(hard_disk, super_block.data_start_lba, buffer, 1); // 将根目录写入 super_block.data_start_lba
                                                                  // super_block.data_start_lba 已经分配给了根目录, 里面是根目录的目录项

    kfree(buffer);
}

void filesystem_init()
{
    uint8_t channel_num = 0;
    uint8_t device_num = 0;
    uint8_t part_idx = 0;

    // super_block_buf 用来存储从硬盘上读入的超级块
    super_block *super_block_buf = (super_block *)kmalloc(SECTOR_SIZE);

    if (super_block_buf == NULL)
    {
        PANIC("alloc memory failed!");
    }

    info;
    console_write("searching filesystem......\n");

    while (channel_num < disk_channel_num)
    {
        device_num = 0;
        while (device_num < 2)
        {

            disk *hard_disk = &channels[channel_num].device[device_num];
            disk_part *part = hard_disk->primary_part;

            while (part_idx < 12)
            {
                if (part_idx == 4) // 开始处理逻辑分区
                {
                    part = hard_disk->logical_part;
                }

                if (part->sector_num != 0) // 如果分区存在
                {

                    memset(super_block_buf, 0, SECTOR_SIZE);

                    // console_write_dec(part->sector_num, black, white);
                    // console_write("\n");

                    disk_read(hard_disk, part->start_LBA + 1, super_block_buf, 1);

                    if (super_block_buf->magic == 0x12348765) // 读出分区的超级块, 根据魔数是否正确来判断是否存在文件系统
                    {
                        info;
                        console_write(part->name);
                        console_write(" has file system \n");
                    }
                    else // 其它文件系统不支持, 一律按无文件系统处理
                    {
                        info;
                        console_write(hard_disk->name);
                        console_write(":");
                        console_write(part->name);
                        console_write(" formatting..");

                        partition_format(part);
                    }
                }
                part_idx++;
                part++; // 下一分区
            }
            device_num++; // 下一磁盘
        }
        channel_num++; // 下一通道
    }

    kfree(super_block_buf);

    // 确定默认操作的分区
    char default_part[8] = "sda1";
    // 挂载分区
    list_traversal(&part_list, mount_partition, (int)default_part);
}
