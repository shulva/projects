#include "../header/stl.h"
#include "../header/heap.h"
#include "../header/string.h"
#include "../header/idt.h"
#include "../header/console.h"

int8_t standard_lessthan(type_void a, type_void b)
{
    return (a < b) ? 1 : 0;
}

ordered_array create_ordered_array(uint32_t max_size, lessthan less_than)
{
    ordered_array ret;

    ret.array = (void *)kmalloc(max_size * sizeof(type_void));
    memset(ret.array, 0, max_size * sizeof(type_void));

    ret.size = 0;
    ret.max_size = max_size;
    ret.less_than = less_than;

    return ret;
}

ordered_array replace_ordered_array(void *addr, uint32_t max_size, lessthan less_than)
{
    ordered_array ret;

    ret.array = (type_void *)addr;
    memset(ret.array, 0, max_size * sizeof(type_void));

    ret.size = 0;
    ret.max_size = max_size;
    ret.less_than = less_than;

    return ret;
}

void destroy_ordered_array(ordered_array *array)
{
    // kfree(array->array);
}

void insert_ordered_array(type_void item, ordered_array *array)
{
    // assert(array->less_than, "lessthan");

    uint32_t iterator = 0;

    while (iterator < array->size && array->less_than(array->array[iterator], item))
    {
        iterator++;
    }

    if (iterator == array->size)
    {
        // console_write("\nitem:");
        // console_write_hex(item, black, white);

        array->array[array->size] = item; // the end of the array

        array->size = array->size + 1;
    }
    else
    {
        type_void tmp = array->array[iterator];
        array->array[iterator] = item;
        while (iterator < array->size)
        {
            iterator++;
            type_void tmp2 = array->array[iterator];
            array->array[iterator] = tmp;
            tmp = tmp2;
        }

        // console_write("\ninsert2:");
        // console_write_hex(array->array[iterator], black, white);

        array->size = array->size + 1;
    }
}

void *search_ordered_array(uint32_t i, ordered_array *array)
{
    assert(i < array->size, "search heap overflow!");

    // for (int k = 0; k < array->size; k++)
    // {
    //     console_write("\n search:");
    //     console_write_hex(array->array[k], black, white);
    // }

    return array->array[i];
}

void remove_ordered_array(uint32_t i, ordered_array *array)
{
    while (i < array->size)
    {
        array->array[i] = array->array[i + 1];
        i++;
    }
    array->size--;
}

void show_array(ordered_array *array)
{
    int i = 0;

    while (i < array->size)
    {
        console_write_hex(array->array[i], black, white);
        console_write("\n");
        i++;
    }
}

////////////////////////////////////////////////
void list_init(double_linked_list *list)
{
    list->head.prev = NULL;
    list->head.next = &list->tail;

    list->tail.prev = &list->head;
    list->tail.next = NULL;
}

void list_insert_before(double_linked_list_node *before, double_linked_list_node *element)
{
    interrupt_status old_status = interrupt_disable();

    before->prev->next = element;

    element->prev = before->prev;
    element->next = before;

    before->prev = element;

    // console_write("\nelement");
    // console_write_hex(element, black, white);
    // console_write("  before");
    // console_write_hex(before, black, white);

    // console_write("\nelement->prev");
    // console_write_hex(element->prev, black, white);
    // console_write("  element->next");
    // console_write_hex(element->next, black, white);

    // console_write("\nbefore->prev");
    // console_write_hex(before->prev, black, white);
    // console_write("  before->next");
    // console_write_hex(before->next, black, white);
    // console_write("\n");

    interrupt_set_status(old_status);
}

void list_push(double_linked_list *list, double_linked_list_node *element)
{
    list_insert_before(list->head.next, element);
}

void list_iterate(double_linked_list *list)
{
}

void list_append(double_linked_list *list, double_linked_list_node *element)
{
    list_insert_before(&list->tail, element);
}

void list_remove(double_linked_list_node *node)
{
    interrupt_status old_status = interrupt_get_status();

    node->prev->next = node->next;
    node->next->prev = node->prev;

    interrupt_set_status(old_status);
}

double_linked_list_node *list_pop(double_linked_list *list)
{
    double_linked_list_node *node = list->head.next;
    list_remove(node);
    return node;
}

_Bool list_empty(double_linked_list *list)
{
    return (list->head.next == &list->tail ? TRUE : FALSE);
}

uint32_t list_len(double_linked_list *list)
{
    double_linked_list_node *node = list->head.next;
    uint32_t length = 0;
    while (node != &list->tail)
    {
        length++;
        node = node->next;
    }
    return length;
}

double_linked_list *list_traversal(double_linked_list *list, function func, int arg)
{
    double_linked_list_node *node = list->head.next;

    if (list_empty(list))
        return NULL;

    while (node != &list->tail)
    {
        if (func(node, arg))
        {
            return node;
        }

        node = node->next;
    }
    return NULL;
}

_Bool node_find(double_linked_list *list, double_linked_list_node *arg_node)
{
    double_linked_list_node *node = &list->tail.prev->prev;

    // console_write("\n");
    // console_write("\nlist->tail.prev->next");
    // console_write_hex(list->tail.prev->next, black, white);
    // console_write("\nnode");
    // console_write_hex(node, black, white);
    // console_write("\nnode-next");
    // console_write_hex(node->next, black, white);
    // console_write("\nlist-tail");
    // console_write_hex(&list->tail, black, white);
    // console_write("\n_arg-node");
    // console_write_hex(arg_node, black, white);
    // console_write("\nlist_len");
    // console_write_hex(list_len(list), black, white);

    while (node != &list->head)
    {
        if (node == arg_node)
        {
            return TRUE;
        }
        // console_write("\nfind:");
        // console_write_hex(node,black,white);
        node = node->prev;
    }

    return FALSE;
}

void list_show(double_linked_list *list)
{
    double_linked_list_node *node = list->head.next;
    // console_write_hex(node,black,white);
    // console_write("\n");
    // console_write_hex(&list->tail.prev->prev,black,white);
    // console_write("\n");

    while (node != &list->tail.prev->prev /*|| node!=0x00000000*/)
    {
        console_write_hex(node, black, white);
        console_write("\n");
        node = node->next;
    }

    console_write_hex(node, black, white);
    console_write("\n");
}
/////////////////////////////////////
void bitmap_int(bitmap *bitmap)
{
    memset(bitmap->bits, 0, bitmap->bitmap_bytes_len);
}

_Bool bitmap_scan_test(bitmap *bitmap, uint32_t bitmap_index)
{
    uint32_t byte_row = bitmap_index / 8;
    uint32_t byte_column = bitmap_index % 8;
    return (bitmap->bits[byte_row] & (BITMAP_MASK << byte_column));
}

int bitmap_scan(bitmap *bitmap, uint32_t num)
{
    uint32_t byte_space_index = 0;

    //找寻空闲位行
    while (0xff == bitmap->bits[byte_space_index] && (byte_space_index < bitmap->bitmap_bytes_len))
    {
        byte_space_index++;
    }

    assert(byte_space_index < bitmap->bitmap_bytes_len, "bitmap overflow");

    if (byte_space_index == bitmap->bitmap_bytes_len)
    {
        return -1;
    }

    int bit_space_index = 0;

    //找寻空闲位列
    while ((uint8_t)BITMAP_MASK << bit_space_index & bitmap->bits[byte_space_index])
    {
        bit_space_index++;
    }

    int space_start = byte_space_index * 8 + bit_space_index;

    if (num == 1)
    {
        return space_start;
    }

    uint32_t bitmap_remain = bitmap->bitmap_bytes_len * 8 - space_start;
    uint32_t next_bit = bit_space_index + 1;
    uint32_t count = 1;

    space_start = -1; //默认找不到连续的

    while (bitmap_remain-- > 0) //搜索剩余的所有位图
    {
        if (!(bitmap_scan_test(bitmap, next_bit)))
        {
            count++;
        }
        else
        {
            count = 0;
        }

        if (count == num)
        {
            space_start = next_bit - num + 1;
            break;
        }

        next_bit++;
    }

    return space_start;
}

void bitmap_set(bitmap *bitmap, uint32_t bitmap_index, int8_t value)
{
    assert((value == 0) || (value == 1), "value is not validate");

    uint32_t byte_row = bitmap_index / 8;
    uint32_t byte_column = bitmap_index % 8;

    if (value)
    {
        bitmap->bits[byte_row] |= (BITMAP_MASK << byte_column); //巧妙..u
    }
    else
    {
        bitmap->bits[byte_row] &= ~(BITMAP_MASK << byte_column);
    }
}