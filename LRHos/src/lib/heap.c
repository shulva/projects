#include "../header/heap.h"
#include "../header/physical_memory.h"
#include "../header/console.h"

extern page_directory *kernel_directory; //在memory.c中

heap *kheap = 0;

uint32_t placement_address = (uint32_t)&kernel_end;

uint32_t kmalloc_int(uint32_t size, int align, uint32_t *physical_addr)
{

    if (kheap != 0)
    {
        void *addr = alloc(size, (uint8_t)align, kheap);
        if (physical_addr != 0)
        {
            page *page = get_page((uint32_t)addr, 0, kernel_directory);
            *physical_addr = page->frame * 0x1000 + (uint32_t)addr & 0xFFF;
        }

        return (uint32_t)addr;
    }
    else
    {
        if (align == 1 && (placement_address & 0xFFFFF000))
        {
            // 页对齐
            placement_address &= 0xFFFFF000;
            placement_address += 0x1000;
        }
        if (physical_addr)
        {
            *physical_addr = placement_address;
        }
        uint32_t tmp = placement_address;
        placement_address += size;
        return tmp;
    }
}

uint32_t kmalloc(uint32_t size)
{
    return kmalloc_int(size, 0, 0);
}

uint32_t kmalloc_align(uint32_t size)
{
    return kmalloc_int(size, 1, 0);
}

uint32_t kmalloc_physical(uint32_t size, uint32_t *physical_addr)
{
    return kmalloc_int(size, 0, physical_addr);
}

uint32_t kmalloc_align_physical(uint32_t size, uint32_t *physical_addr)
{
    return kmalloc_int(size, 1, physical_addr);
}

void kfree(void *p)
{
    free(p, kheap);
}

static int8_t header_less_than(void *a, void *b)
{
    // console_write("\n(header *)a)->size");
    // console_write_hex(((header *)a)->size, black, white);
    // console_write("\n(header *)b)->size");
    // console_write_hex(((header *)b)->size, black, white);

    return (((header *)a)->size < ((header *)b)->size) ? 1 : 0;
}

static uint32_t contract(uint32_t new_size, heap *heap)
{

    assert(new_size < heap->end_address - heap->start_address, "contract: size expand but not contract");

    if (new_size & 0x1000)
    {
        new_size &= 0x1000;
        new_size += 0x1000;
    }

    if (new_size < HEAP_MIN_SIZE)
        new_size = HEAP_MIN_SIZE;

    uint32_t old_size = heap->end_address - heap->start_address;
    uint32_t i = old_size - 0x1000;
    while (new_size < i)
    {
        free_page(get_page(heap->start_address + i, 0, kernel_directory));
        i -= 0x1000;
    }

    heap->end_address = heap->start_address + new_size;
    return new_size;
}

static int32_t find_smallest_block(uint32_t size, uint8_t page_align, heap *heap) // Find the smallest free block that will fit.
{

    uint32_t iterator = 0;

    // console_write("\n");

    // console_write_hex(heap, black, white);
    // console_write_hex(heap->index.size, black, white);

    while (iterator < heap->index.size)
    {

        header *Header = (header *)search_ordered_array(iterator, &heap->index);
        // console_write("\n");
        // console_write_hex(Header->size, black, white);

        if (page_align > 0)
        {
            uint32_t location = (uint32_t)Header;
            int32_t offset = 0;

            uint32_t new_location = location + 0x1000 - (location & 0xFFF) - sizeof(header);
            header *page_block = (header *)new_location;

            // console_write("\n");
            // console_write_hex(location, black, white);
            // console_write("\n");

            // console_write_hex(new_location, black, white);
            // console_write("\n");

            // console_write_hex(page_block->magic, black, white);

            if ((location + sizeof(header)) & 0xFFFFF000 != 0)          // 实际的内存块地址
                offset = 0x1000 - (location + sizeof(header)) % 0x1000; //类似于两者互补，相加后为页对齐状态

            int32_t block_size = (int32_t)Header->size - offset; //为页对齐补上offset长度的内存，从block里补

            if (page_block->is_free == 0 && page_block->magic == 0x12345678)
            {
                iterator++;
                continue;
            } // gg,next

            if (block_size >= (int32_t)size)
            {
                break;
            } // 匹配
        }
        else if (Header->size >= size) // 匹配
            break;

        iterator++;
    }

    if (iterator == heap->index.size)
        return -1; // We got to the end and didn't find anything.
    else
        return iterator;
}

// heap初始化
heap *create_heap(uint32_t start, uint32_t end_addr, uint32_t max, uint8_t supervisor, uint8_t readonly)
{
    heap *Heap = (heap *)kmalloc(sizeof(Heap));

    assert(start % 0x1000 == 0, "start addr not be page-aligned");

    assert(end_addr % 0x1000 == 0, "end addr not be page-aligned");

    Heap->index = replace_ordered_array((void *)KHEAP_START, HEAP_INDEX, &header_less_than); // 0xc0000000 alloc的地址一定要与 heap->index的地址分开

    // Write the start, end and max addresses into the heap structure.
    Heap->start_address = start;
    Heap->end_address = end_addr;
    Heap->max_address = max;
    Heap->supervisor = supervisor;
    Heap->readonly = readonly;

    return Heap;
}

static void expand(uint32_t new_size, heap *heap)
{

    assert(new_size > heap->end_address - heap->start_address, "\n heap expand :not expand but smaller");

    // Get the nearest following page boundary.
    if (new_size & 0xFFFFF000 != 0)
    {
        new_size &= 0xFFFFF000;
        new_size += 0x1000;
    }

    assert(heap->start_address + new_size <= heap->max_address, "\n heap expand :overflow heap maxszie");

    // This should always be on a page boundary.
    uint32_t old_size = heap->end_address - heap->start_address;

    uint32_t i = old_size;
    while (i < new_size)
    {
        allocate_page(get_page(heap->start_address + i, 1, kernel_directory),
                      (heap->supervisor) ? 1 : 0, (heap->readonly) ? 0 : 1);
        i += 0x1000;
    }
    heap->end_address = heap->start_address + new_size;
}

// size的单位是1byte
void *alloc(uint32_t size, uint8_t page_align, heap *heap)
{

    uint32_t new_size = size + sizeof(header) + sizeof(footer);

    int32_t iterator = find_smallest_block(new_size, page_align, heap);

    // console_write_hex(iterator,black,white);

    if (iterator == -1) // 没找到适合的空块
    {

        uint32_t old_length = heap->end_address - heap->start_address;
        uint32_t old_end_address = heap->end_address;

        expand(old_length + new_size, heap); //分配新的空间
        uint32_t new_length = heap->end_address - heap->start_address;

        // Find the endmost header. (Not endmost in size, but in location).
        iterator = 0;

        uint32_t idx = -1;
        uint32_t value = 0x0;
        while (iterator < heap->index.size)
        {
            uint32_t tmp = (uint32_t)search_ordered_array(iterator, &heap->index);
            if (tmp > value)
            {
                value = tmp;
                idx = iterator;
            }
            iterator++;
        }

        // If we didn't find ANY headers, we need to add one.
        if (idx == -1)
        {
            header *Header = (header *)old_end_address;
            Header->magic = HEAP_MAGIC;
            Header->size = new_length - old_length - sizeof(header);
            Header->is_free = 1;

            footer *Footer = (footer *)(old_end_address + Header->size - sizeof(footer));
            Footer->magic = HEAP_MAGIC;
            Footer->header = Header;

            insert_ordered_array((void *)Header, &heap->index);
        }
        else
        {
            // we find that ,and the last header needs adjusting.
            header *Header = search_ordered_array(idx, &heap->index);
            Header->size += new_length - old_length;

            // Rewrite the footer.
            footer *Footer = (footer *)((uint32_t)Header + Header->size - sizeof(footer));
            Footer->header = Header;
            Footer->magic = HEAP_MAGIC;
        }

        return alloc(size, page_align, heap); // 调整过后就可以了
    }

    header *fit_block_header = (header *)search_ordered_array(iterator, &heap->index);

    uint32_t fit_block_position = (uint32_t)fit_block_header;
    uint32_t fit_block_size = fit_block_header->size;

    // console_write("\nfit_block_position:");
    // console_write_hex(fit_block_position, black, white);
    // console_write("\n");

    // Here we work out if we should split the hole we found into two parts.
    // Is the original block size - requested block size less than the overhead for adding a new hole?
    if (fit_block_size - new_size < sizeof(header) + sizeof(footer))
    {
        // 大小连header+footer都不够，不能增加新块，只能扩充size大小
        size += fit_block_size - new_size;
        new_size = fit_block_size;
    }

    if (page_align && fit_block_position & 0xFFFFF000)
    {
        //你可能需要画个图来理解具体的位置情况

        uint32_t new_location = fit_block_position + 0x1000 - (fit_block_position & 0xFFF) - sizeof(header);

        //页对齐后的空出来的内容不能浪费，建一个block来存着
        header *block_header = (header *)fit_block_position;

        block_header->size = 0x1000 - (fit_block_position & 0xFFF) - sizeof(header);
        block_header->magic = HEAP_MAGIC;
        block_header->is_free = 1;

        footer *block_footer = (footer *)((uint32_t)new_location - sizeof(footer));

        block_footer->magic = HEAP_MAGIC;
        block_footer->header = block_header;

        insert_ordered_array((void *)block_header, &heap->index);
        remove_ordered_array(iterator, &heap->index);

        //调整位置
        fit_block_position = new_location;
        fit_block_size = fit_block_size - block_header->size; //似乎不能保证fit_block_size > block_header->size 啊。。

        // console_write("new_location:");
        // console_write_hex(new_location, black, white);
        // console_write("\n");
    }
    else
    {
        remove_ordered_array(iterator, &heap->index);
    }

    // Overwrite the original header and footer ...
    header *replace_block_header = (header *)fit_block_position;
    replace_block_header->magic = HEAP_MAGIC;
    replace_block_header->is_free = 0;
    replace_block_header->size = new_size - sizeof(header);

    footer *block_footer = (footer *)(fit_block_position + sizeof(header) + size);
    block_footer->magic = HEAP_MAGIC;
    block_footer->header = replace_block_header;

    // console_write("\nheader:");
    // console_write_hex(replace_block_header, black, white);
    // console_write("\nheader->magic:");
    // console_write_hex(replace_block_header->magic, black, white);
    // console_write("\nfooter->magic:");
    // console_write_hex(block_footer->magic, black, white);
    // console_write("\nfooter->header->magic:");
    // console_write_hex(block_footer->header->magic, black, white);
    // console_write("\n");

    // We may need to write a new hole after the allocated block.
    // We do this only if the new hole would have positionitive size...
    //多余空间再利用

    if (fit_block_size - new_size > 0) // header+footer的空间都不够的情况已经在前面的判断中去除了
    {
        header *more_block_header = (header *)(fit_block_position + new_size);

        more_block_header->magic = HEAP_MAGIC;
        more_block_header->is_free = 1;
        more_block_header->size = fit_block_size - new_size - sizeof(header);

        footer *hole_footer = (footer *)((uint32_t)more_block_header + fit_block_size - new_size - sizeof(footer));

        if ((uint32_t)hole_footer < heap->end_address)
        {
            hole_footer->magic = HEAP_MAGIC;
            hole_footer->header = more_block_header;
        }

        // console_write("\nmore_block_header");
        // console_write_hex(more_block_header, black, white);

        insert_ordered_array((void *)more_block_header, &(heap->index));
    }

    replace_block_header->magic = HEAP_MAGIC;

    // console_write("\nshowarray:");
    // show_array(&heap->index);

    return (void *)((uint32_t)replace_block_header + sizeof(header));
}

void free(void *p, heap *heap)
{
    if (p == 0)
        return;

    header *Header = (header *)((uint32_t)p - sizeof(header));
    footer *Footer = (footer *)((uint32_t)Header + Header->size - sizeof(footer) + sizeof(header));

    // console_write("\nHeader:");
    // console_write_hex(Header, black, white);
    // console_write("\nHeader->magic:");
    // console_write_hex(Header->magic, black, white);
    // console_write("\nHeader->is_free:");
    // console_write_hex(Header->is_free, black, white);
    // console_write("\nHeader->size:");
    // console_write_hex(Header->size, black, white);
    // console_write("\nFooter:");
    // console_write_hex(Footer, black, white);
    // console_write("\nFooter->magic:");
    // console_write_hex(Footer->magic, black, white);
    // console_write("\nFooter->header->magic:");
    // console_write_hex(Footer->header->magic, black, white);
    // console_write("\n");

    assert(Header->magic == HEAP_MAGIC, "\nheader magic num wrong");
    assert(Footer->magic == HEAP_MAGIC, "\nfooter magic num wrong");

    Header->is_free = 1;

    // Do we want to add this header into the 'free holes' index?
    int8_t do_add_to_heap_index = 1;

    // Unify left//////////////////////////////////////////
    // If the thing immediately to the left of us is a footer...
    footer *before_footer = (footer *)((uint32_t)Header - sizeof(footer));

    if (before_footer->magic == HEAP_MAGIC && before_footer->header->is_free == 1)
    {
        uint32_t tmp = Header->size;
        Header = before_footer->header;
        Footer->header = Header;
        Header->size += tmp;

        do_add_to_heap_index = 0; // Since this header is already in the index, we don't want to add it again.
    }

    // Unify right/////////////////////////////////////////
    // If the thing immediately to the right of us is a header...
    header *after_header = (header *)((uint32_t)Footer + sizeof(footer));

    if (after_header->magic == HEAP_MAGIC && after_header->is_free)
    {
        Header->size += after_header->size;
        Footer = (footer *)((uint32_t)after_header + after_header->size - sizeof(footer));

        // Find and remove this header from the index.
        uint32_t iterator = 0;
        while ((iterator < heap->index.size) && (search_ordered_array(iterator, &heap->index) != (void *)after_header))
            iterator++;

        assert(iterator < heap->index.size, "free: not found in heap index ");

        remove_ordered_array(iterator, &heap->index);
    }

    // If the footer location is the end address, we can contract the heap.
    if ((uint32_t)Footer + sizeof(footer) == heap->end_address)
    {
        uint32_t old_length = heap->end_address - heap->start_address;
        uint32_t new_length = contract((uint32_t)Header - heap->start_address, heap);

        // Check how big we will be after resizing.
        if (Header->size - (old_length - new_length) > 0)
        {
            Header->size -= old_length - new_length; // size可以容纳内存大小的变化，此为变化后的大小

            Footer = (footer *)((uint32_t)Header + Header->size - sizeof(footer));
            Footer->magic = HEAP_MAGIC;
            Footer->header = Header;
        }
        else
        {
            // We will no longer exist , Remove us from the index.
            uint32_t iterator = 0;
            while ((iterator < heap->index.size) && (search_ordered_array(iterator, &heap->index) != (void *)after_header))
                iterator++;

            if (iterator < heap->index.size)
                remove_ordered_array(iterator, &heap->index);
        }
    }

    if (do_add_to_heap_index == 1)
        insert_ordered_array((void *)Header, &heap->index);
}