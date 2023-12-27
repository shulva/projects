#ifndef HEADER_PAGING_H
#define HEADER_PAGING_H

#include "../header/string.h"
#include "../header/idt.h"

uint32_t *pages; // save pages
uint32_t num_page;

extern uint32_t placement_address;

#define index(a) (a / (8 * 4))
#define offset(a) (a % (8 * 4))

// 32 bit
typedef struct page
{
    uint32_t P_present : 1;
    // Set if the page is present in memory

    uint32_t R_W : 1;
    // If set, that page is writeable. If unset, the page is read-only. This does not apply when code is running in kernel-mode (unless a flag in CR0 is set).

    uint32_t U_S : 1;
    // If set, this is a user-mode page. Else it is a supervisor (kernel)-mode page. User-mode code cannot write to or read from kernel-mode pages.

    uint32_t A_accessed : 1;
    // Set if the page has been accessed (Gets set by the CPU).

    uint32_t D_dirty : 1;
    // Set if the page has been written to (dirty).

    uint32_t unused : 7;
    // total of unused and reserved bits(2+2+3)

    uint32_t frame : 20;
    // page address (shifted right 12 bits)

} page;

typedef struct page_table
{
    page pages[1024];
} page_table;

typedef struct page_directory
{

    // Array of pointers to pagetables.
    page_table *tables[1024];

    // Array of pointers to the pagetables above, but gives their *physical *location, for loading into the CR3 register.
    uint32_t tablesPhysical[1024];

    /**
    The physical address of tablesPhysical. This comes into play
    when we get our kernel heap allocated and the directory
    may be in a different location in virtual memory.
    **/
    uint32_t physicalAddr;

} page_directory;

// Sets up the environment, page directories etc and enables paging.
void init_memory();

// Causes the specified page directory to be loaded into the CR3 register.
// Copy the location of your page directory into the CR3 register. This must, of course, be the physical address.
void switch_page_directory(page_directory *dir);

/**
Retrieves a pointer to the page required.
If make == 1, if the page-table in which this page should
reside isn't created, create it!
**/
page *get_page(uint32_t address, int make, page_directory *dir);

// Handler for page faults.
void page_fault(isr_reg regs);

// the bitmap of pages
void set_page(uint32_t page_addr);

void clear_page(uint32_t page_addr);

uint32_t test_page(uint32_t page_addr);

uint32_t first_free_page();

uint32_t allocate_page(page *page, int is_kernel, int read_write);

uint32_t free_page(page *page);

#endif