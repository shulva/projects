#include "../header/timer.h"
#include "../header/console.h"
#include "../header/idt.h"
#include "../header/thread.h"

#define irq_frequency 100

#define ms_per_interrupt (1000 / irq_frequency) //一次中断10ms左右

uint32_t tick = 0;

void timer_callback(isr_reg *regs)
{
    thread_pcb *current_thread = get_running_thread_pcb();

    // console_write("current_thread\n");
    // console_write_hex(current_thread,black,white);
    // console_write("current_thread->magic\n");
    // console_write_hex(current_thread->magic,black,white);
    // console_write("\n");

    // assert(current_thread->magic == 0x87654321, "thread stack magic question!");

    if (current_thread->magic != 0x87654321)
    {
        return;
        // console_write("\ncurrent_thread:");
        // console_write_hex(current_thread,black,white);
        // console_write("\ncurrent_thread->magic:");
        // console_write_hex(current_thread->magic,black,white);
    }

    current_thread->done_ticks++;
    tick++;

    if (current_thread->ticks == 0)
    {
        // console_write("\ncurrent_thread->ticks:");
        // console_write_hex(current_thread->ticks,black,white);
        schedule();
    }
    else
    {
        current_thread->ticks--;
    }
}

void init_timer(uint32_t frequency)
{
    // 注册时间相关的处理函数
    init_interrupt_func(32, &timer_callback);

    // 输入频率为 1193180，frequency 即每秒中断次数
    uint32_t divisor = 1193180 / frequency;

    // sets the PIT to repeating mode
    outbyte(0x43, 0x36);

    // 拆分低字节和高字节
    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t hign = (uint8_t)((divisor >> 8) & 0xFF);

    // 分别写入低字节和高字节
    outbyte(0x40, low);
    outbyte(0x40, hign);
}

//以tick为单位的sleep
static void ticks_to_sleep(uint32_t sleep_ticks)
{
    uint32_t start_ticks = tick;

    while (tick - start_ticks < sleep_ticks)
    {
        thread_yield();
        tick++;
    }
}
//以ms为单位的sleep
void ms_sleep(uint32_t ms)
{
    uint32_t sleep_ticks = ms / ms_per_interrupt; //转换成中断次数
    assert(sleep_ticks > 0, "sleep ticks <0");
    ticks_to_sleep(sleep_ticks);
}

void loop_sleep(uint8_t times)
{
    for (int i = 0; i < times; i++)
    {
        for (int j = 0; j < 100000; j++)
        {
            i++;
            i--;
        }
    }
}
