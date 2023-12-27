#include "../header/idt.h"
#include "../header/string.h"
#include "../header/console.h"

#define length 256
#define EFLAGS_IF 0x00000200 // if在flags的第10位(15,14...0的顺序)
#define GET_FLAGS(eflags) asm volatile("pushfl; popl %0 " \
                                       : "=g"(eflags))

idt_seg idt[length];

idtr idt_table;

static void idt_set(uint8_t num, uint32_t base, uint16_t limit, uint8_t flags);

static char *interrupt_name[32];

void init_interrupt_func(uint8_t num, interrupt_handler func)
{
    interrupt_func[num] = func;
}

void init_idt()
{

    //         command  data
    // 主片端口 0x20    0x21
    // 从片端口 0xA0    0xA1

    // 初始化主片master、从片slave
    // 0001 0001

    outbyte(0x20, 0x11);
    outbyte(0xA0, 0x11);

    // 设置主片 IRQ 从 0x20(32)号中断开始
    outbyte(0x21, 0x20);

    // 设置从片 IRQ 从 0x28(40=32+8)号中断开始
    outbyte(0xA1, 0x28);

    // 设置主片 IR2 引脚连接从片
    outbyte(0x21, 0x04);

    // 告诉从片输出引脚和主片 IR2 号相连
    outbyte(0xA1, 0x02);

    // 设置主片和从片按照 8086 的方式工作
    outbyte(0x21, 0x01);
    outbyte(0xA1, 0x01);

    // 设置主从片允许中断
    outbyte(0x21, 0x0);
    outbyte(0xA1, 0x0);

    idt_table.limit = sizeof(idt_seg) * 256 - 1;
    idt_table.base = (uint32_t)&idt;

    memset(&idt, 0, sizeof(idt_seg) * 256);

    // isr
    idt_set(0, (uint32_t)isr0, 0x08, 0x8E);
    idt_set(1, (uint32_t)isr1, 0x08, 0x8E);
    idt_set(2, (uint32_t)isr2, 0x08, 0x8E);
    idt_set(3, (uint32_t)isr3, 0x08, 0x8E);
    idt_set(4, (uint32_t)isr4, 0x08, 0x8E);
    idt_set(5, (uint32_t)isr5, 0x08, 0x8E);
    idt_set(6, (uint32_t)isr6, 0x08, 0x8E);
    idt_set(7, (uint32_t)isr7, 0x08, 0x8E);
    idt_set(8, (uint32_t)isr8, 0x08, 0x8E);
    idt_set(9, (uint32_t)isr9, 0x08, 0x8E);
    idt_set(10, (uint32_t)isr10, 0x08, 0x8E);
    idt_set(11, (uint32_t)isr11, 0x08, 0x8E);
    idt_set(12, (uint32_t)isr12, 0x08, 0x8E);
    idt_set(13, (uint32_t)isr13, 0x08, 0x8E);
    idt_set(14, (uint32_t)isr14, 0x08, 0x8E);
    idt_set(15, (uint32_t)isr15, 0x08, 0x8E);
    idt_set(16, (uint32_t)isr16, 0x08, 0x8E);
    idt_set(17, (uint32_t)isr17, 0x08, 0x8E);
    idt_set(18, (uint32_t)isr18, 0x08, 0x8E);
    idt_set(19, (uint32_t)isr19, 0x08, 0x8E);
    idt_set(20, (uint32_t)isr20, 0x08, 0x8E);
    idt_set(21, (uint32_t)isr21, 0x08, 0x8E);
    idt_set(22, (uint32_t)isr22, 0x08, 0x8E);
    idt_set(23, (uint32_t)isr23, 0x08, 0x8E);
    idt_set(24, (uint32_t)isr24, 0x08, 0x8E);
    idt_set(25, (uint32_t)isr25, 0x08, 0x8E);
    idt_set(26, (uint32_t)isr26, 0x08, 0x8E);
    idt_set(27, (uint32_t)isr27, 0x08, 0x8E);
    idt_set(28, (uint32_t)isr28, 0x08, 0x8E);
    idt_set(29, (uint32_t)isr29, 0x08, 0x8E);
    idt_set(30, (uint32_t)isr30, 0x08, 0x8E);
    idt_set(31, (uint32_t)isr31, 0x08, 0x8E);

    // irq
    idt_set(32, (uint32_t)irq0, 0x08, 0x8E);
    idt_set(33, (uint32_t)irq1, 0x08, 0x8E);
    idt_set(34, (uint32_t)irq2, 0x08, 0x8E);
    idt_set(35, (uint32_t)irq3, 0x08, 0x8E);
    idt_set(36, (uint32_t)irq4, 0x08, 0x8E);
    idt_set(37, (uint32_t)irq5, 0x08, 0x8E);
    idt_set(38, (uint32_t)irq6, 0x08, 0x8E);
    idt_set(39, (uint32_t)irq7, 0x08, 0x8E);
    idt_set(40, (uint32_t)irq8, 0x08, 0x8E);
    idt_set(41, (uint32_t)irq9, 0x08, 0x8E);
    idt_set(42, (uint32_t)irq10, 0x08, 0x8E);
    idt_set(43, (uint32_t)irq11, 0x08, 0x8E);
    idt_set(44, (uint32_t)irq12, 0x08, 0x8E);
    idt_set(45, (uint32_t)irq13, 0x08, 0x8E);
    idt_set(46, (uint32_t)irq14, 0x08, 0x8E);
    idt_set(47, (uint32_t)irq15, 0x08, 0x8E);

    idt_flush((uint32_t)&idt_table);

    for (int i = 0; i < 32; i++)
    {
        interrupt_name[i] = "unhandled interrupt";
    }

    interrupt_name[0] = "divide error";
    interrupt_name[1] = "debug exception";
    interrupt_name[2] = "nmi interrupt";
    interrupt_name[3] = "breakpoint exception ";
    interrupt_name[4] = "overflow exception";
    interrupt_name[5] = "bound range exceeded exception";
    interrupt_name[6] = "invalide opcode exception";
    interrupt_name[7] = "device not available exception";
    interrupt_name[8] = "double fault exception";
    interrupt_name[9] = "co-processor segment overrun";
    interrupt_name[10] = "invalid tss exception";
    interrupt_name[11] = "segment not exist";
    interrupt_name[12] = "stack fault exception";
    interrupt_name[13] = "general protection exception";
    interrupt_name[14] = "page fault exception";
    interrupt_name[15] = "intel reserved";
    interrupt_name[16] = "x87-floating point error";
    interrupt_name[17] = "alignment check exception";
    interrupt_name[18] = "machine-check exception";
    interrupt_name[19] = "simd floating point exception";
}

static void idt_set(uint8_t num, uint32_t base, uint16_t seg_select, uint8_t flags)
{
    idt[num].offset_low = base & 0xffff;
    idt[num].offset_high = (base >> 16) & 0xffff;

    idt[num].seg_select = seg_select;
    idt[num].zero = 0;
    idt[num].flags = flags; // user mode need logical or 0x60
}

void isr_handle(isr_reg *regs)
{
    if (regs->int_num >= 40)
    {
        outbyte(0xA0, 0x20); // 发送重设信号给从片
    }

    outbyte(0x20, 0x20); // 发送重设信号给主片

    if (interrupt_func[regs->int_num])
    {
        interrupt_func[regs->int_num](regs);
    }
    else
    {
        //其实应该实现一个printf函数的。。
        console_write("\n");
        console_write_color("[EXCEPTION OR ERROR]:", black, red);
        console_write_color(interrupt_name[regs->int_num], black, white);
        for (;;)
            ;
    }
}

// before you return from an IRQ handler, you must inform the PIC that you have
// finished, so it can dispatch the next (if there is one waiting). This is known as an EOI (end of
// interrupt)(中断结束信号)
// If the master PIC sent the IRQ (number 0-7),
// we must send an EOI to the master (obviously). If the slave sent the IRQ (8-15), we must send
// an EOI to both the master and the slave (because of the daisy-chaining of the two).
void irq_handle(isr_reg *regs)
{
    // 按照我们的设置，从 32 号中断起为用户自定义中断
    // 因为单片的 Intel 8259A 芯片只能处理 8 级中断
    // 故大于等于 40 的中断号是由从片处理的

    if (regs->int_num >= 40)
    {
        outbyte(0xA0, 0x20); // 发送重设信号给从片
    }

    outbyte(0x20, 0x20); // 发送重设信号给主片

    if (interrupt_func[regs->int_num])
    {
        interrupt_func[regs->int_num](regs);
    }
}

interrupt_status interrupt_get_status(void)
{
    uint32_t eflags = 0;
    GET_FLAGS(eflags);
    return (eflags & EFLAGS_IF) ? INTR_ON : INTR_OFF;
}

interrupt_status interrupt_set_status(interrupt_status status)
{
    return status & INTR_ON ? interrupt_enable() : interrupt_disable();
}

//开中断并return返回开中断前的状态
interrupt_status interrupt_enable(void)
{
    interrupt_status old_status;
    if (INTR_ON == interrupt_get_status())
    {
        old_status = INTR_ON;
        return old_status;
    }
    else
    {
        old_status = INTR_OFF;
        asm volatile("sti"); //开中断
        return old_status;
    }
}

//关中断并return返回关中断前的状态
interrupt_status interrupt_disable(void)
{
    interrupt_status old_status;
    if (INTR_ON == interrupt_get_status())
    {
        old_status = INTR_ON;
        /*
        假如一个内联汇编语句的Clobber/Modify域存在"memory"，
        那么GCC会保证在此内联汇编之前，假如某个内存的内容被装进了寄存器，
        那么在这个内联汇编之后，假如需要使用这个内存处的内容，
        就会直接到这个内存处重新读取，而不是使用被存放在寄存器中的拷贝
        */
        asm volatile("cli" ::
                         : "memory");

        return old_status;
    }
    else
    {
        old_status = INTR_OFF;
        return old_status;
    }
}