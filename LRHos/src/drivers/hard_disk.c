#include "../header/hard_disk.h"

// for the different port in ata channels
#define reg_data(channel) (channel->port + 0)
#define reg_error(channel) (channel->port + 1)
#define reg_sector_num(channel) (channel->port + 2)
#define reg_lba_low(channel) (channel->port + 3)
#define reg_lba_mid(channel) (channel->port + 4)
#define reg_lba_high(channel) (channel->port + 5)
#define reg_device(channel) (channel->port + 6)
#define reg_status(channel) (channel->port + 7)
#define reg_alternate_status(channel) (channel->port + 0x206)

#define reg_cmd(channel) (reg_status(channel))
#define reg_ctl(channel) (reg_alternate_status(channel))

// alt_reg的关键位信息
#define bit_alt_reg_bsy 0x80  //硬盘忙，勿扰
#define bit_alt_reg_drdy 0x40 //设备就绪，等待指令
#define bit_alt_reg_drq 0x8   // 硬盘已准备好数据，随时可以输出
// reg_device的关键位信息
#define bit_device_mbs 0xa0
#define bit_device_lba 0x40
#define bit_device_dev 0x10
//硬盘操作命令
#define cmd_identify 0xec     // identify指令
#define cmd_read_sector 0x20  //读扇区指令
#define cmd_write_sector 0x30 //写扇区指令

#define max_sector ((80 * 1024 * 1024 / 512) - 1) // 80mb

void disk_init()
{

    uint8_t disk_num = *((uint8_t *)(0x475));

    assert(disk_num > 0, "no disk exist");

    console_write("\n");
    info;
    console_write("disk init.. disk num is ");
    console_write_dec(disk_num, black, white);
    console_write("\n");

    disk_channel_num = (disk_num / 2) + 1;

    disk_channel *channel;
    uint8_t channel_number = 0;

    int device_num = 0;
    list_init(&part_list);

    while (channel_number < disk_channel_num)
    {
        channel = &channels[channel_number];

        info;
        console_write("channel num:");
        // console_write(channel->name);
        // console_put_color(':', black, white);
        console_write_dec(channel_number, black, white);
        console_write("\n");

        switch (channel_number)
        {
        case 0:
            channel->port = 0x1f0; // ide0通道起始端口
            channel->interrupt_num = 0x20 + 14;
            break;
        case 1:
            channel->port = 0x170; // ide1通道起始端口
            channel->interrupt_num = 0x20 + 15;
            break;
        }

        channel->interrupt_expect = FALSE;
        init_interrupt_func(channel->interrupt_num, interrupt_hard_disk_handler);

        lock_init(&channel->lock);
        semaphore_init(&channel->semaphore, 0);

        while (device_num < disk_num)
        {

            disk *hard_disk = &channel->device[device_num];
            hard_disk->mychannel = channel;
            hard_disk->dev_num = device_num;

            // for (int i = 0; i < 6; i++)
            // {
            //     boot_sector *ebr = kmalloc(sizeof(boot_sector));

            //     disk_read(hard_disk, test[i], ebr, 1);

            //     console_write_hex(ebr->signature, black, white);
            // }

            hard_disk->name[0] = 's';
            hard_disk->name[1] = 'd';
            hard_disk->name[2] = 'a' + channel_number * 2 + device_num;

            identify_disk(hard_disk);
            partition_scan(hard_disk, 0);

            primary_part_num = logical_part_num = 0;
            device_num++;
        }

        device_num = 0;

        channel_number++;
    }

    console_write("\n");
    info;
    console_write("all part info:");
    list_traversal(&part_list, partition_info, (int)NULL);

    console_write("\n");
    info;
    console_write("disk init done");
    console_write("\n");
}

static void select_disk(disk *hard_disk)
{

    uint8_t reg_device = bit_device_mbs | bit_device_lba;

    if (hard_disk->dev_num == 1)
    {
        reg_device |= bit_device_dev;
    }

    outbyte(reg_device(hard_disk->mychannel), reg_device);
}

static void select_sector(disk *hard_disk, uint32_t lba, uint8_t sector_num)
{
    // console_write_dec(lba, black, white);
    // console_write(" ");
    // console_write_dec(sector_num, black, white);
    // console_write(" ");

    assert(lba < max_sector, "overflow boundary");

    disk_channel *channel = hard_disk->mychannel;

    //写入扇区数
    outbyte(reg_sector_num(channel), sector_num);

    //写入lba,即扇区号
    outbyte(reg_lba_low(channel), lba);
    outbyte(reg_lba_mid(channel), lba >> 8);
    outbyte(reg_lba_high(channel), lba >> 16);

    outbyte(reg_device(channel), bit_device_mbs | bit_device_lba | (hard_disk->dev_num == 1 ? bit_device_dev : 0) | lba >> 24);
}

static void cmd_out(disk_channel *channel, uint8_t cmd)
{
    //向硬盘发出命令便置为true
    channel->interrupt_expect = TRUE;
    outbyte(reg_cmd(channel), cmd);
}

static void read_from_sector(disk *hard_disk, void *buffer, uint8_t sector_num)
{
    uint32_t size_of_byte;
    if (sector_num == 0) //若为0，其实为256 (0xff+1=0x00)
    {
        size_of_byte = 256 * 512;
    }
    else
    {
        size_of_byte = sector_num * 512;
    }

    // 1word=2byte ,so, size_of_byte / 2
    insw(reg_data(hard_disk->mychannel), buffer, size_of_byte / 2);
}

static void write_to_sector(disk *hard_disk, void *buffer, uint8_t sector_num)
{
    uint32_t size_of_byte;
    if (sector_num == 0) //若为0，其实为256
    {
        size_of_byte = 256 * 512;
    }
    else
    {
        size_of_byte = sector_num * 512;
    }

    // 1word=2byte ,so, size_of_byte / 2

    outsw(reg_data(hard_disk->mychannel), buffer, size_of_byte / 2);
}

// wait for 30s...
static _Bool busy_wait(disk *hard_disk)
{

    disk_channel *channel = hard_disk->mychannel;

    uint16_t time = 30 * 1000; // 30s

    while (time -= 10 >= 0)
    {
        if (!(inbyte(reg_status(channel)) & bit_alt_reg_bsy))
        {
            return inbyte(reg_status(channel)) & bit_alt_reg_drq;
        }
        else
        {
            ms_sleep(10); // 10ms
        }
    }

    return FALSE;
}

// read sector_num sector from disk to buffer
void disk_read(disk *hard_disk, uint32_t lba, void *buffer, uint32_t sector_num)
{
    assert(lba < max_sector, "overflow boundary");
    assert(sector_num > 0, "sector_num <=0");

    lock_acquire(&hard_disk->mychannel->lock);

    //选择要操作的硬盘
    select_disk(hard_disk);

    // console_write("\n");
    // console_write_dec(lba, black, white);

    uint32_t sector_operate_num;  // 每次操作的扇区数
    uint32_t sector_done_num = 0; // 已完成的扇区数

    while (sector_done_num < sector_num)
    {
        if ((sector_done_num + 256) <= sector_num)
        {
            sector_operate_num = 256;
        }
        else
        {
            sector_operate_num = sector_num - sector_done_num;
        }

        //写入待读入的扇区数和起始扇区号
        select_sector(hard_disk, lba + sector_done_num, sector_operate_num);

        //将执行的命令写入reg_cmd寄存器
        cmd_out(hard_disk->mychannel, cmd_read_sector);

        // console_write(" data code:");
        // console_write_hex(inbyte(reg_data(hard_disk->mychannel)), black, white);
        // console_write("\n");

        // console_write(" error code:");
        // console_write_hex(inbyte(reg_error(hard_disk->mychannel)), black, white);
        // console_write("\n");

        // console_write(" sector_num code:");
        // console_write_hex(inbyte(reg_sector_num(hard_disk->mychannel)), black, white);
        // console_write("\n");

        // console_write(" device code:");
        // console_write_hex(inbyte(reg_device(hard_disk->mychannel)), black, white);
        // console_write("\n");

        // console_write(" status code:");
        // console_write_hex(inbyte(reg_status(hard_disk->mychannel)), black, white);
        // console_write("\n");

        // console_write(" alternate_status code:");
        // console_write_hex(inbyte(reg_alternate_status(hard_disk->mychannel)), black, white);
        // console_write("\n");

        semaphore_sub(&hard_disk->mychannel->semaphore);

        //检测硬盘状态

        //其实这有个bug没有解决,你把下面这段被注释掉的代码恢复就可以看到了。

        if (!busy_wait(hard_disk))
        {
            console_write("\n");
            info;
            console_write(hard_disk->name);
            console_write(":");
            console_write_dec(lba, black, white);
            console_write(" error code:");
            console_write_hex(inbyte(reg_error(hard_disk->mychannel)), black, white);

            // PANIC("read disk error");
        }

        read_from_sector(hard_disk, (void *)((uint32_t)buffer + sector_done_num * 512), sector_operate_num);
        sector_done_num = sector_done_num + sector_operate_num;
    }

    lock_release(&hard_disk->mychannel->lock);
}

// read sector_num sector from buffer to disk
void disk_write(disk *hard_disk, uint32_t lba, void *buffer, uint32_t sector_num)
{

    assert(lba < max_sector, "overflow boundary");
    assert(sector_num > 0, "sector_num <=0");

    lock_acquire(&hard_disk->mychannel->lock);

    //选择要操作的硬盘
    select_disk(hard_disk);

    // console_write("\n");
    // console_write_hex(hard_disk, black, white);
    // console_write("\n");
    // console_write_hex(lba, black, white);
    // console_write("\n");
    // console_write_hex(buffer, black, white);
    // console_write("\n");
    // console_write_hex(sector_num, black, white);
    // console_write("\n");

    uint32_t sector_operate_num;  // 每次操作的扇区数
    uint32_t sector_done_num = 0; // 已完成的扇区数

    while (sector_done_num < sector_num)
    {
        if ((sector_done_num + 256) <= sector_num)
        {
            sector_operate_num = 256;
        }
        else
        {
            sector_operate_num = sector_num - sector_done_num;
        }

        //写入待读入的扇区数和起始扇区号
        select_sector(hard_disk, lba + sector_done_num, sector_operate_num);
        //将执行的命令写入reg_cmd寄存器
        cmd_out(hard_disk->mychannel, cmd_write_sector);

        //检测硬盘状态
        if (!busy_wait(hard_disk))
        {
            console_write("\n");
            info;
            console_write(hard_disk->name);
            console_write(":");
            console_write_dec(lba, black, white);
            PANIC("write disk error");
        }

        // console_write_dec(hard_disk->mychannel->semaphore.value, black, white);
        write_to_sector(hard_disk, (void *)((uint32_t)buffer + sector_done_num * 512), sector_operate_num);

        semaphore_sub(&hard_disk->mychannel->semaphore); // blocked

        // console_write(" data code:");
        // console_write_hex(inbyte(reg_data(hard_disk->mychannel)), black, white);
        // console_write("\n");

        // console_write(" error code:");
        // console_write_hex(inbyte(reg_error(hard_disk->mychannel)), black, white);
        // console_write("\n");

        // console_write(" sector_num code:");
        // console_write_hex(inbyte(reg_sector_num(hard_disk->mychannel)), black, white);
        // console_write("\n");

        // console_write(" device code:");
        // console_write_hex(inbyte(reg_device(hard_disk->mychannel)), black, white);
        // console_write("\n");

        // console_write(" status code:");
        // console_write_hex(inbyte(reg_status(hard_disk->mychannel)), black, white);
        // console_write("\n");

        // console_write(" alternate_status code:");
        // console_write_hex(inbyte(reg_alternate_status(hard_disk->mychannel)), black, white);
        // console_write("\n");

        sector_done_num = sector_done_num + sector_operate_num; //循环
    }

    lock_release(&hard_disk->mychannel->lock);
}

//硬盘中断处理程序
void interrupt_hard_disk_handler(isr_reg *regs)
{
    assert(regs->int_num == 0x2e || regs->int_num == 0x2f, "disk int_num wrong");
    // console_write("disk intr");
    uint8_t channel_number = regs->int_num - 0x20 - 14; // disk_init()
    disk_channel *channel = &channels[channel_number];

    assert(channel->interrupt_num == regs->int_num, "channel_int_num not eauals regs_int_num");
    if (channel->interrupt_expect)
    {
        channel->interrupt_expect = FALSE;
        semaphore_add(&channel->semaphore);
        inbyte(reg_status(channel));
    }
}

static void identify_disk(disk *hard_disk)
{
    char disk_information[512];
    select_disk(hard_disk);

    cmd_out(hard_disk->mychannel, cmd_identify);

    /* 向硬盘发送指令后便通过信号量阻塞自己,
       待硬盘处理完成后, 通过中断处理程序将自己唤醒 */
    semaphore_sub(&hard_disk->mychannel->semaphore);

    if (!busy_wait(hard_disk))
    {
        console_write("\n");
        info;
        console_write(hard_disk->name);
        PANIC("indentify disk error");
    }

    // the data returned is 2bytes,1word
    read_from_sector(hard_disk, disk_information, 1);

    char buffer[64];

    uint8_t serial_num_start = 10 * 2, serial_num_len = 20, type_num_start = 27 * 2, type_num_len = 40;

    info;
    console_write("disk name:");
    console_write(hard_disk->name);

    swap_bytes(&disk_information[serial_num_start], buffer, serial_num_len);

    console_write("\n");
    info;
    console_write("serial_num:");
    console_write(buffer);

    memset(buffer, 0, sizeof(buffer));

    swap_bytes(&disk_information[type_num_start], buffer, type_num_len);

    console_write("\n");
    info;
    console_write("type:");
    console_write(buffer);

    uint32_t sector_num = *(uint32_t *)&disk_information[60 * 2];

    console_write("\n");
    info;
    console_write("sector_num:");
    console_write_dec(sector_num, black, white);

    console_write("\n");
    info;
    console_write("total:");
    console_write_dec(sector_num * 512 / 1024 / 1024, black, white);
    console_write("mb");
}

static void partition_scan(disk *hard_disk, uint32_t ext_lba)
{

    boot_sector *ebr = kmalloc(sizeof(boot_sector));

    disk_read(hard_disk, ext_lba, ebr, 1);

    uint8_t part_index = 0;

    part *part_table = ebr->part_table;

    // console_write("\n");
    // console_write_dec(part_table[0].start_lba, black, white);
    // console_write("\n");
    // console_write_dec(part_table[1].start_lba, black, white);
    // console_write("\n");
    // console_write_dec(part_table[2].start_lba, black, white);
    // console_write("\n");
    // console_write_dec(part_table[3].start_lba, black, white);

    while (part_index++ < 4)
    {
        // console_write("\nlba:");
        // console_write_dec(part_table->start_lba, black, white);
        // console_write("\ntype:");
        // console_write_hex(part_table->part_type, black, white);

        if (part_table->part_type == 0x05) // 若为扩展分区
        {

            if (ext_lba_base != 0) //子扩展分区的 start-lba 是相对于主引导扇区中的总扩展分区地址
            {
                // console_write("\npart_table->start_lba:");
                // console_write_dec(part_table->start_lba, black, white);
                // console_write("\next_lba_base:");
                // console_write_dec(ext_lba_base, black, white);

                partition_scan(hard_disk, part_table->start_lba + ext_lba_base);
            }
            else
            // ext_lba_base = 0 表示是第一次读取引导 也就是主引导记录所在的扇区
            // 记录下扩展分区的起始 lba地址, 后面所有的扩展分区地址都相对于此
            {
                ext_lba_base = part_table->start_lba;
                partition_scan(hard_disk, part_table->start_lba);
            }
        }
        else if (part_table->part_type != 0)
        {
            if (ext_lba == 0) //主扇区
            {
                hard_disk->primary_part[primary_part_num].start_LBA = ext_lba + part_table->start_lba;
                hard_disk->primary_part[primary_part_num].sector_num = part_table->sector_num;
                hard_disk->primary_part[primary_part_num].mydisk = hard_disk;

                hard_disk->primary_part[primary_part_num].name[0] = hard_disk->name[0];
                hard_disk->primary_part[primary_part_num].name[1] = hard_disk->name[1];
                hard_disk->primary_part[primary_part_num].name[2] = hard_disk->name[2];
                hard_disk->primary_part[primary_part_num].name[3] = '1' + primary_part_num;

                list_append(&part_list, &hard_disk->primary_part[primary_part_num].part_tag);

                // console_write("\n");
                // info;
                // console_write(hard_disk->name);
                // // console_write("hard_disk_part_name:");
                // // console_write(hard_disk->primary_part[primary_part_num].name);
                // console_write_dec(primary_part_num + 1, black, white);

                primary_part_num++;
                assert(primary_part_num < 4, "primary_part_num overflow");
            }
            else //逻辑扇区
            {
                hard_disk->logical_part[logical_part_num].name[0] = hard_disk->name[0];
                hard_disk->logical_part[logical_part_num].name[1] = hard_disk->name[1];
                hard_disk->logical_part[logical_part_num].name[2] = hard_disk->name[2];
                hard_disk->logical_part[logical_part_num].name[3] = '5' + logical_part_num;

                hard_disk->logical_part[logical_part_num].start_LBA = ext_lba + part_table->start_lba;
                hard_disk->logical_part[logical_part_num].sector_num = part_table->sector_num;
                hard_disk->logical_part[logical_part_num].mydisk = hard_disk;

                list_append(&part_list, &hard_disk->logical_part[logical_part_num].part_tag);

                // console_write("\n");
                // info;
                // console_write(hard_disk->name);
                // // console_write("hard_disk_part_name:");
                // // console_write(hard_disk->logical_part[logical_part_num].name);
                // console_write_dec(logical_part_num + 5, black, white);

                logical_part_num++;
                if (logical_part_num >= 8)
                {
                    return;
                }
            }
        }

        part_table++;
    }

    kfree(ebr);
}

static _Bool partition_info(double_linked_list_node *part_elem, int unused)
{
    disk_part *part = element_entry(disk_part, part_tag, part_elem);

    console_write("\n");
    info;

    console_write(part->name);
    console_write(" lba:");
    console_write_dec(part->start_LBA, black, white);
    console_write(" sector_num:");
    console_write_dec(part->sector_num, black, white);

    return FALSE;
}
