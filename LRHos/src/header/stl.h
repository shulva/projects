#ifndef HEADER_STL_H
#define HEADER_STL_H

#include "types.h"

////////
// ordered array

// A predicate should return nonzero if the first argument is less than the second. Else it should return zero.

typedef void *type_void;

typedef int8_t (*lessthan)(type_void, type_void);

typedef struct ordered_array
{
    type_void *array;
    uint32_t size;
    uint32_t max_size;
    lessthan less_than;
} ordered_array;

int8_t standard_lessthan(type_void a, type_void b);

ordered_array create_ordered_array(uint32_t max_size, lessthan less_than);

ordered_array replace_ordered_array(void *addr, uint32_t max_size, lessthan less_than);

void destroy_ordered_array(ordered_array *array);

// insert操作
void insert_ordered_array(void *item, ordered_array *array);

// search操作
void *search_ordered_array(uint32_t i, ordered_array *array);

// remove操作
void remove_ordered_array(uint32_t i, ordered_array *array);

void show_array(ordered_array *array);
///////////////////////////////////////

#define offset_for_stl(struct_type, element) (int)(&((struct_type *)0)->element) //结构体中相关变量的偏移

#define element_entry(struct_type, struct_element_name, element_pointer) \
    (struct_type *)((int)element_pointer - offset_for_stl(struct_type, struct_element_name))

//双向链表
struct double_linked_list_node
{
    struct double_linked_list_node *prev;
    struct double_linked_list_node *next;
};

typedef struct double_linked_list_node double_linked_list_node;

struct double_linked_list
{
    double_linked_list_node head; //头结点
    double_linked_list_node tail; //尾结点
};

typedef struct double_linked_list double_linked_list;

typedef _Bool(function)(double_linked_list_node *, int arg);

//初始化
void list_init(double_linked_list *);

//把Node插入在元素before之前
void list_insert_before(double_linked_list_node *before, double_linked_list_node *element);

// push元素到列表首位
void list_push(double_linked_list *list, double_linked_list_node *element);

void list_iterate(double_linked_list *list);

// append元素到列表队尾
void list_append(double_linked_list *list, double_linked_list_node *element);

//删除元素
void list_remove(double_linked_list_node *node);

//将链表第一个元素返回，类似栈的pop
double_linked_list_node *list_pop(double_linked_list *list);

//判断链表是否为空
_Bool list_empty(double_linked_list *list);

//返回链表长度
uint32_t list_len(double_linked_list *list);

//把链表中的每一个元素的node和arg传给回调函数，判断是否有符合"条件"的元素
double_linked_list *list_traversal(double_linked_list *list, function func, int arg);

//在链表中查找元素
_Bool node_find(double_linked_list *list, double_linked_list_node *node);

// show elements
void list_show(double_linked_list *list);

/////////////////////////////////
//位图
#define BITMAP_MASK 1

typedef struct bitmap
{
    uint32_t bitmap_bytes_len;
    uint8_t *bits; //以位为单位
} bitmap;

//初始化
void bitmap_init(bitmap *bitmap);
//判断此位是否为1.是则ture,不是则false
_Bool bitmap_scan_test(bitmap *bitmap, uint32_t bitmap_index);
//在位图中申请连续个num位
int bitmap_scan(bitmap *bitmap, uint32_t num);
//将位图bitmap上的index位设置为相应值
void bitmap_set(bitmap *bitmap, uint32_t bitmap_index, int8_t value);

#endif