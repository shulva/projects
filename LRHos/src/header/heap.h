#ifndef HEADER_HEAP_H
#define HEADER_HEAP_H

#include "../header/types.h"
#include "../header/stl.h"

#define HEAP_MAGIC 0x12345678
#define HEAP_INDEX 0x10000
#define HEAP_MIN_SIZE 0x70000
#define KHEAP_START 0xC0000000
#define KHEAP_INITIAL_SIZE 0x100000

uint32_t kmalloc_int(uint32_t size, int align, uint32_t *phys);

uint32_t kmalloc(uint32_t size);                                         //  (normal).
uint32_t kmalloc_align(uint32_t size);                                   // page aligned.
uint32_t kmalloc_physical(uint32_t size, uint32_t *physical_addr);       // returns a physical address.
uint32_t kmalloc_align_physical(uint32_t size, uint32_t *physical_addr); // page aligned and returns a physical address.

void kfree(void *p);

typedef struct header // sizeof(header)=0x0c
{
    uint32_t magic;
    uint8_t is_free;
    uint32_t size; // size of the block, including the end footer.
} header;

typedef struct footer // sizeof(footer)=0x08
{
    uint32_t magic;
    header *header; // Pointer to the block header.
} footer;

typedef struct heap
{
    ordered_array index;
    uint32_t start_address; // The start of our allocated space.
    uint32_t end_address;   // The end of our allocated space. May be expanded up to max_address.
    uint32_t max_address;   // The maximum address the heap can be expanded to.
    uint8_t supervisor;     // Should extra pages requested by us be mapped as supervisor-only?
    uint8_t readonly;       // Should extra pages requested by us be mapped as read-only?
} heap;

/**
   Allocates a contiguous region of memory 'size' in size. If page_align==1, it creates that block starting
   on a page boundary.
**/
heap *create_heap(uint32_t start, uint32_t end_addr, uint32_t max, uint8_t supervisor, uint8_t readonly);

void *alloc(uint32_t size, uint8_t page_align, heap *heap);

void free(void *p, heap *heap);

#endif