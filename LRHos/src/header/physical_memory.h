#ifndef HEADER_PHYSICAL_MEMORY_H
#define HEADER_PHYSICAL_MEMORY_H

#include "../header/types.h"
#include "../header/paging.h"

// 内核文件在内存中的起始和结束位置
extern uint8_t kernel_start[];
extern uint8_t kernel_end[]; // linker script

void show_memory_map();

uint32_t allocate_page(page *page, int is_kernel, int read_write);

#endif
