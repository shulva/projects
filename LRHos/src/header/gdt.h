#ifndef HEADER_GDT_H
#define HEADER_GDT_H

#include "types.h"

// gdt的具体设计请参考
//《Intel® 64 and IA-32 Architectures Software Developer’s Manual Vol-ume3 : System Programming Guide》

// segment descriptor 段描述符
typedef struct gdt_entry
{
    uint16_t seg_limit;       //段界限 0-15
    uint16_t base_low;        //段基地址 0-15
    uint8_t base_middle;      //段基地址 16-23
    uint8_t access_byte;      // flags(P,DPL,DT,type)
    uint8_t granularity_byte; // flags(G,D,0,A),段界限 16-19
    uint8_t base_high;        //段基址 24-31
} __attribute__((packed)) gdt_seg;

//__attribute__ ((packed))让编译器取消结构在编译过程中的优化对齐,按照实际占用字节数进行对齐

// gdtr
typedef struct gdt_table
{
    uint16_t limit; //全局描述符表长
    uint32_t base;  //全局描述符表32位基地址
} __attribute__((packed)) gdtr;

void init_gdt();

// asm中将gdt加载到gdtr的函数
extern void gdt_flush(uint32_t);

#endif