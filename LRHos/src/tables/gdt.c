#include "../header/gdt.h"

#define length 5

gdt_seg gdt[length];

gdtr gdt_table;

static void gdt_set(int16_t num, uint32_t base, uint32_t limit, uint8_t access_byte, uint8_t gran);

// extern uint32_t stack;

void init_gdt()
{

    gdt_table.limit = sizeof(gdt) * length - 1;
    gdt_table.base = (uint32_t)&gdt;

    gdt_set(0, 0, 0, 0, 0);
    gdt_set(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // cs
    gdt_set(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // ds
    gdt_set(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // user_mode_cs
    gdt_set(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // user_mode_ds

    gdt_flush((uint32_t)&gdt_table);
}

static void gdt_set(int16_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    gdt[num].base_low = (base & 0xffff);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    gdt[num].seg_limit = (limit & 0xFFFF);

    gdt[num].granularity_byte = (limit >> 16) & 0x0F;
    gdt[num].granularity_byte = (gdt[num].granularity_byte) | (gran & 0xF0);

    gdt[num].access_byte = access;
}