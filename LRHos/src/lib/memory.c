#include "../header/multiboot.h"
#include "../header/console.h"
#include "../header/physical_memory.h"
#include "../header/paging.h"
#include "../header/heap.h"

// The kernel's page directory
page_directory *kernel_directory = 0;

// The current page directory;
page_directory *current_directory = 0;

// the max size of physical memory(512mb)
#define PHYSICAL_PAGE_MAX_SIZE 0x20000000
// the size of a page (4kb)
#define PAGE_SIZE 0x1000
// num of page
#define NUM_OF_PAGE (PHYSICAL_PAGE_MAX_SIZE / PAGE_SIZE)

uint32_t page_num;

uint32_t physical_memory_page_stack[NUM_OF_PAGE];

uint32_t physical_memory_page_stack_pointer;

extern heap *kheap;

void show_memory_map()
{
    uint32_t addr = glb_mboot_ptr->memory_map_addr;
    uint32_t length = glb_mboot_ptr->memory_map_length;

    console_write("memory map:\n");

    memory_map *mmap = (memory_map *)addr;
    for (mmap = (memory_map *)addr; (uint32_t)mmap < addr + length; mmap++)
    {
        // console_write_dec((uint32_t)mmap->size,light_green,white);
        console_write("base addr=");
        console_write_hex((uint32_t)mmap->base_addr_high, black, white);
        console_write(":");
        console_write_hex((uint32_t)mmap->base_addr_low, black, white);
        console_write(" length=");
        console_write_hex((uint32_t)mmap->length_high, black, white);
        console_write(":");
        console_write_hex((uint32_t)mmap->length_low, black, white);
        console_write(" type=");
        console_write_hex((uint32_t)mmap->type, black, white);
        console_write("\n");
    }
}

void init_memory()
{

    physical_memory_page_stack_pointer = 0;
    num_page = NUM_OF_PAGE;

    pages = (uint32_t *)kmalloc(index(num_page));
    memset(pages, 0, index(num_page));

    kernel_directory = (page_directory *)kmalloc_align(sizeof(page_directory));
    memset(kernel_directory, 0, sizeof(page_directory));
    current_directory = kernel_directory;

    // heap
    int i = 0;
    for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += 0x1000)
        get_page(i, 1, kernel_directory);

    i = 0;
    while (i < placement_address)
    {
        // Kernel code is readable but not writeable from userspace.
        allocate_page(get_page(i, 1, kernel_directory), 0, 0);
        i += PAGE_SIZE;
    }

    allocate_page(get_page(i, 1, kernel_directory), 0, 1); // 0x1000这个多余的页是为了储存访问heap而使用的
    i += PAGE_SIZE;

    console_write("\n");
    info;
    console_write_color("the num of physical page is:", black, white);
    console_write_dec(i / PAGE_SIZE, black, white);

    for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += PAGE_SIZE) // 256 pages
        allocate_page(get_page(i, 1, kernel_directory), 0, 1);

    init_interrupt_func(14, page_fault);

    uint32_t kstart = KHEAP_START;
    kstart += sizeof(void *) * HEAP_INDEX; // 0xc0000000-0xc0080000 use for heap itself

    if (kstart & 0xFFFFF000 != 0)
    {
        kstart &= 0xFFFFF000;
        kstart += 0x1000;
    }

    kheap = create_heap(kstart, KHEAP_START + KHEAP_INITIAL_SIZE, 0xCFFFF000, 0, 0);

    console_write("\n");
    info;
    console_write("kheap init addr:");
    console_write_hex(kheap, black, white);

    switch_page_directory(kernel_directory);

    header *block = (header *)kstart;
    block->size = KHEAP_INITIAL_SIZE;
    block->magic = HEAP_MAGIC;
    block->is_free = 1;

    insert_ordered_array((void *)block, &(kheap->index));

    console_write("\n");
    info;
    console_write("pointer num:");
    console_write_dec(physical_memory_page_stack_pointer, black, white); // page_tables not count

    console_write("\n");
    info;
    console_write_color("the addr is:", black, white);
    console_write_hex(placement_address, black, white);

    /*
        // memory_map *start_addr=(memory_map *)glb_mboot_ptr->memory_map_addr;
        // memory_map *end_addr=(memory_map *)glb_mboot_ptr->memory_map_addr+glb_mboot_ptr->memory_map_length;

        // memory_map *entry ;

        // for (entry=start_addr; entry < end_addr; entry++)
        // {
        //     //please check out the comments of memory_map
        //     if((entry->type)== 1  && entry->base_addr_low==0x100000)
        //     {
        //         uint32_t physical_page_start_addr = entry->base_addr_low +(uint32_t)(kernel_end-kernel_start);
        //         uint32_t length = entry ->base_addr_low + entry->length_low;

        //         while (physical_page_start_addr<length && physical_page_start_addr < PHYSICAL_PAGE_MAX_SIZE)
        //         {
        //             //allocate_page(physical_page_start_addr);//memory become paging
        //             physical_page_start_addr+=PAGE_SIZE;
        //             page_num++;
        //         }

        //     }
        // }

        // console_write("test free page: ");
        // console_write("\nfree page memory addr:");
        // console_write_dec(free_page(),black,white);

        // int position=free_page();
        // console_write("\nfree page memory addr:");
        // console_write_dec(position,black,white);

        // console_write("\ntest allocate page:");
        // console_write("\nallocate page memory addr:");
        // console_write_dec(allocate_page(position),black,white);
        // console_write("\nallocatr page memory addr:");
        // console_write_dec(allocate_page(position+0x1000),black,white);
    */
}

uint32_t free_page(page *page)
{
    uint32_t position;
    if (!(position = page->frame))
    {
        return; // The given page didn't actually have an allocated space
    }
    else
    {
        clear_page(position); // Frame is now free again.
        page->frame = 0x0;    // Page now doesn't exist
    }

    assert(physical_memory_page_stack_pointer != 0, "overflow page memory border");

    uint32_t memory = physical_memory_page_stack[physical_memory_page_stack_pointer--]; // pop

    return memory;
}

uint32_t allocate_page(page *page, int is_kernel, int read_write)
{
    if (page->frame != 0)
    {
        return; // was allocated
    }
    else
    {
        uint32_t index = first_free_page();
        if (0xffffffff == index)
        {
            console_write("no free space\n");
        }

        set_page(index * 0x1000);

        page->P_present = 1;              // it is present, or not
        page->R_W = (read_write) ? 1 : 0; // Should the page be writeable?
        page->U_S = (is_kernel) ? 0 : 1;  // Should the page be user-mode?
        page->frame = index;

        assert(physical_memory_page_stack_pointer != PHYSICAL_PAGE_MAX_SIZE, "page stack overflow");

        physical_memory_page_stack[++physical_memory_page_stack_pointer] = index * 0x1000; // push

        return physical_memory_page_stack[physical_memory_page_stack_pointer];
    }
}

void set_page(uint32_t page_addr)
{
    uint32_t page = page_addr / 0x1000;
    uint32_t idx = index(page);
    uint32_t off = offset(page);
    pages[idx] |= (0x1 << off);
}

void clear_page(uint32_t page_addr)
{
    uint32_t page = page_addr / 0x1000;
    uint32_t idx = index(page);
    uint32_t off = offset(page);
    pages[idx] &= ~(0x1 << off); // logical and with zero = zero
}

uint32_t test_page(uint32_t page_addr)
{
    uint32_t page = page_addr / 0x1000;
    uint32_t idx = index(page);
    uint32_t off = offset(page);
    return (pages[idx] & (0x1 << off)); // test if a bit is set.
}

uint32_t first_free_page()
{
    uint32_t i, j;
    for (i = 0; i < index(num_page); i++) // num_page/32=uint32_t num of pages
    {
        if (pages[i] != 0xFFFFFFFF) // nothing free, exit early.
        {
            // at least one bit is free here.
            for (j = 0; j < 32; j++)
            {
                uint32_t logical_and_bit = 0x1 << j;
                if (!(pages[i] & logical_and_bit))
                {
                    return i * 4 * 8 + j; // return the chosen
                }
            }
        }
    }
}

page *get_page(uint32_t address, int make, page_directory *dir)
{

    address /= 0x1000; // Turn the address into the index.

    uint32_t table_index = address / 1024; // Find the page table containing this address.

    if (dir->tables[table_index]) // If this table is already assigned
    {
        return &dir->tables[table_index]->pages[address % 1024];
    }
    else if (make)
    {
        uint32_t tmp;
        dir->tables[table_index] = (page_table *)kmalloc_align_physical(sizeof(page_table), &tmp);
        dir->tablesPhysical[table_index] = tmp | 0x7; // PRESENT, RW, US.
        return &dir->tables[table_index]->pages[address % 1024];
    }
    else
    {
        return 0;
    }
}

void switch_page_directory(page_directory *dir)
{
    current_directory = dir;
    asm volatile("mov %0, %%cr3" ::"r"(&dir->tablesPhysical));
    uint32_t cr0;
    asm volatile("mov %%cr0, %0"
                 : "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging!
    asm volatile("mov %0, %%cr0" ::"r"(cr0));
}

void page_fault(isr_reg regs)
{
    // A page fault has occurred.
    // The faulting address is stored in the CR2 register.
    uint32_t faulting_address;
    asm volatile("mov %%cr2, %0"
                 : "=r"(faulting_address));

    // The error code gives us details of what happened.
    int present = !(regs.err_code & 0x1); // Page not present
    int rw = regs.err_code & 0x2;         // Write operation?
    int us = regs.err_code & 0x4;         // Processor was in user-mode?
    int reserved = regs.err_code & 0x8;   // Overwritten CPU-reserved bits of page entry?
    int id = regs.err_code & 0x10;        // Caused by an instruction fetch?

    // Output an error message.

    console_write_color("[EXCEPTION OR ERROR]:", black, light_red);
    console_write("Page fault!(");
    if (present)
    {
        console_write("present ");
    }
    if (rw)
    {
        console_write("read-only ");
    }
    if (us)
    {
        console_write("user-mode ");
    }
    if (reserved)
    {
        console_write("reserved ");
    }

    console_write(") at ");
    console_write_hex(faulting_address, black, white);
    console_write("\n");

    PANIC("page fault location");
}
