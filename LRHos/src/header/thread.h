#ifndef HEADER_THREAD_H
#define HEADER_THREAD_H

#include "types.h"
#include "stl.h"
#include "string.h"
#include "console.h"
#include "paging.h"
#include "heap.h"

typedef void thread_func(void *);

enum task_status
{
    TASK_RUNNING, //运行
    TASK_READY,   //就绪
    TASK_BLOCKED, //阻塞
    TASK_WAITING, //等待
    TASK_HANGING, //悬挂
    TASK_DIED     // gg
};

//保护程序的上下文环境。进程或线程被中断打断时，会按照此结构体压入上下文
//此栈在线程的内核栈中位置固定，在页的最顶端
typedef struct interrupt_stack
{
    uint32_t edi; // 从 edi 到 eax 由 pusha 指令压入 , 在interrupt.s中
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    uint32_t int_num;  // 中断号
    uint32_t err_code; // 错误代码(有中断错误代码的中断会由CPU压入)
    uint32_t eip;      // 以下由处理器自动压入
    uint32_t cs;
    uint32_t eflags;
    uint32_t useresp;
    uint32_t ss;
} interrupt_stack;

//线程栈，用于储存线程栈待执行的函数
typedef struct thread_stack
{
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edi;
    uint32_t esi;

    void (*eip)(thread_func *func, void *func_arg); //函数地址

    void(*return_addr); // 返回地址
    thread_func *func;
    void *func_arg;
} thread_stack;

typedef struct thread_pcb
{
    uint32_t *itself_stack; //各个内核线程的内核栈
    enum task_status status;
    char name[16];
    uint8_t priority;

    uint32_t ticks;                  //每次在cpu上的执行次数(时间片)，优先级越高，执行的时间越长
    uint32_t done_ticks;             // thread在cpu上占用的执行次数
    double_linked_list_node tag;     //线程在就绪队列中的结点
    double_linked_list_node all_tag; //线程在全线程队列中的结点
    uint32_t *page_dir;
    // uint32_t pid;

    uint32_t magic; //栈的边界
} thread_pcb;

double_linked_list ready_thread_list; //就绪队列
double_linked_list all_thread_list;   //所有队列

thread_pcb *thread_start(char *name, int priority, thread_func func, void *func_arg);

void init_thread(thread_pcb *pthread, char *name, int priority);

void thread_create(thread_pcb *pthread, thread_func func, void *func_arg);

void thread_init(void);

thread_pcb *get_running_thread_pcb();
//任务调度
void schedule();
// lock
void thread_blocked(enum task_status status);
// unlock
void thread_unblocked(thread_pcb *pthread);
// yield cpu
void thread_yield();
#endif