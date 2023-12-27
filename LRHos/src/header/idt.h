#ifndef HEADER_IDT_H
#define HEADER_IDT_H

#include "types.h"

// 跟gdt差不多
typedef struct idt_entry
{
    uint16_t offset_low; //中断处理函数地址 15 ～ 0 位
    uint16_t seg_select; // 目标代码段描述符选择子
    uint8_t zero;        // 置 0 段
    uint8_t flags;
    uint16_t offset_high; // 中断处理函数地址 31 ～ 16 位
} __attribute__((packed)) idt_seg;

//__attribute__ ((packed))让编译器取消结构在编译过程中的优化对齐,按照实际占用字节数进行对齐

typedef struct idt_table
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idtr;

//为什么参数的顺序是这样? 请参考interrupt.s中参数压入的顺序
typedef struct isr_reg
{
    uint32_t edi; // 从 edi 到 eax 由 pusha 指令压入 , 在interrupt.s中
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    // ds,es,fs,gs还是得考虑一下
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    uint32_t int_num;  // 中断号
    uint32_t err_code; // 错误代码(有中断错误代码的中断会由CPU压入)
    uint32_t eip;      // 以下由处理器自动压入
    uint32_t cs;
    uint32_t eflags;

    uint32_t useresp; //特权级
    uint32_t ss;
} isr_reg;

typedef void (*interrupt_handler)(isr_reg *); //中断处理函数指针,统一放在函数指针数组中
interrupt_handler interrupt_func[256];        //等价于 void (*interrupt_func[256])(isr_reg *)

void init_idt();

void init_interrupt_func(uint8_t num, interrupt_handler func);

extern void idt_flush(uint32_t);

//////////////////////////////////////////////
void isr_handle(isr_reg *regs);

void isr0();  // 0 #DE 除 0 异常
void isr1();  // 1 #DB 调试异常
void isr2();  // 2 NMI
void isr3();  // 3 BP 断点异常
void isr4();  // 4 #OF 溢出
void isr5();  // 5 #BR 对数组的引用超出边界
void isr6();  // 6 #UD 无效或未定义的操作码
void isr7();  // 7 #NM 设备不可用(无数学协处理器)
void isr8();  // 8 #DF 双重故障(有错误代码)
void isr9();  // 9 协处理器跨段操作
void isr10(); // 10 #TS 无效TSS(有错误代码)
void isr11(); // 11 #NP 段不存在(有错误代码)
void isr12(); // 12 #SS 栈错误(有错误代码)
void isr13(); // 13 #GP 常规保护(有错误代码)
void isr14(); // 14 #PF 页故障(有错误代码)
void isr15(); // 15 CPU 保留
void isr16(); // 16 #MF 浮点处理单元错误
void isr17(); // 17 #AC 对齐检查
void isr18(); // 18 #MC 机器检查
void isr19(); // 19 #XM SIMD(单指令多数据)浮点异常

// 20 ~ 31 Intel 保留
void isr20();
void isr21();
void isr22();
void isr23();
void isr24();
void isr25();
void isr26();
void isr27();
void isr28();
void isr29();
void isr30();
void isr31();
////////////////////////////////////////////////

void irq_handle(isr_reg *regs);

// irq外部中断请求 32-47
void irq0();  // 电脑系统计时器,timer
void irq1();  // 键盘
void irq2();  // 与 IRQ9 相接，MPU-401 MD 使用
void irq3();  // 串口设备
void irq4();  // 串口设备
void irq5();  // 建议声卡使用
void irq6();  // 软驱传输控制使用
void irq7();  // 打印机传输控制使用
void irq8();  // 即时时钟
void irq9();  // 与 IRQ2 相接，可设定给其他硬件
void irq10(); // 建议网卡使用
void irq11(); // 建议 AGP 显卡使用
void irq12(); // 接 PS/2 鼠标，也可设定给其他硬件
void irq13(); // 协处理器使用
void irq14(); // IDE0 传输控制使用
void irq15(); // IDE1 传输控制使用

// 32 ~ 255 用户自定义异常
void isr255();

////////////////////////////

typedef enum interrupt_status
{
    INTR_OFF,
    INTR_ON
} interrupt_status;

interrupt_status interrupt_get_status(void);
interrupt_status interrupt_set_status(interrupt_status);
interrupt_status interrupt_enable(void);
interrupt_status interrupt_disable(void);

#endif