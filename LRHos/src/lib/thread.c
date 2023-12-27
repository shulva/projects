#include "../header/thread.h"

#define PAGE_SIZE 0x1000

extern page_directory *kernel_directory;

extern uint32_t placement_address; // heap.c

thread_pcb *main_thread; //主线程Pcb
thread_pcb *disk_thread; //硬盘pcb

static double_linked_list_node *thread_tag; //保存队列中的线程结点

extern void switch_thread(thread_pcb *pcb, thread_pcb *next_pcb);

uint32_t pid_num = 0;

//系统空闲时运行的线程
static void disk(void *arg)
{
    while (1)
    {
        thread_blocked(TASK_BLOCKED);
        asm volatile("sti;hlt" ::
                         : "memory");
    }
}

thread_pcb *get_running_thread_pcb()
{
    uint32_t esp;
    asm("mov %%esp ,%0"
        : "=g"(esp));

    return (thread_pcb *)(esp & 0xfffff000); //取当前栈指针的前20位，作为当前运行线程的pcb
}

static void kernel_thread(thread_func *func, void *func_arg)
{
    // console_write("thread.c-kernel_thread");
    interrupt_enable(); //开中断以免时钟中断被屏蔽
    func(func_arg);
}

void thread_create(thread_pcb *pthread, thread_func func, void *func_arg)
{
    pthread->itself_stack = pthread->itself_stack - sizeof(interrupt_stack); //预留出中断栈的空间

    pthread->itself_stack = pthread->itself_stack - sizeof(thread_stack); //预留线程栈的空间

    thread_stack *kernel_thread_stack = (thread_stack *)pthread->itself_stack;

    kernel_thread_stack->eip = kernel_thread;

    kernel_thread_stack->func = func;
    kernel_thread_stack->func_arg = func_arg;

    kernel_thread_stack->ebp = 0;
    kernel_thread_stack->ebx = 0;
    kernel_thread_stack->esi = 0;
    kernel_thread_stack->edi = 0;
}

void init_thread(thread_pcb *pthread, char *name, int priority)
{
    // console_write_hex(main_thread->all_tag.next,black,white);

    memset(pthread, 0, sizeof(thread_pcb));

    // console_write_hex(main_thread->all_tag.next,black,white);

    strcpy(pthread->name, name);
    console_write(pthread->name);
    // console_write(name);

    if (pthread == main_thread)
    {
        pthread->status == TASK_RUNNING; //主线程一直在跑
    }
    else
    {
        pthread->status = TASK_READY;
    }

    pthread->itself_stack = (uint32_t *)((uint32_t)pthread + PAGE_SIZE);
    pthread->ticks = priority;
    pthread->page_dir = NULL; //线程没有自己的地址空间
    pthread->done_ticks = 0;  //尚未运行
    pthread->priority = priority;
    // pthread->pid = pid_num;
    pthread->magic = 0x87654321;

    pid_num++;
}

thread_pcb *thread_start(char *name, int priority, thread_func func, void *func_arg)
{
    thread_pcb *thread = kmalloc_align(sizeof(thread_pcb)); // get_page(placement_address, 1, kernel_directory);

    // console_write("\nthread:");
    // console_write_hex(thread,black,white);

    // allocate_page(get_page(placement_address, 1, kernel_directory),0,1);

    // placement_address += PAGE_SIZE;

    init_thread(thread, name, priority);

    thread_create(thread, func, func_arg);

    // console_write_hex(&ready_thread_list.head, black, white);
    // console_write_hex(&ready_thread_list.tail, black, white);
    // console_write("\n");
    // console_write_hex(&all_thread_list.head, black, white);
    // console_write_hex(&all_thread_list.tail, black, white);

    // console_write_hex(thread,black,white);
    // console_write("\n");
    // console_write("\n");
    // list_show(&ready_thread_list);
    // console_write("\n");

    // console_write_hex(&thread->tag.next,black,white);
    // console_write_hex(&thread->tag.prev,black,white);
    // console_write("\n");
    // console_write_hex(&thread->all_tag.next,black,white);
    // console_write_hex(&thread->all_tag.prev,black,white);
    // console_write("\n");

    // console_write_hex(&thread->tag,black,white);
    // console_write_hex(&thread->all_tag,black,white);

    assert(!node_find(&ready_thread_list, &thread->tag), "\nrepeat in ready thread list");
    list_append(&ready_thread_list, &thread->tag);

    assert(!node_find(&all_thread_list, &thread->all_tag), "\nrepeat in all thread list");
    list_append(&all_thread_list, &thread->all_tag);

    // asm volatile("movl %0, %%esp; pop %%ebp ; pop %%ebx; pop %%edi; pop %%esi; ret"
    //              :
    //              : "g"(thread->itself_stack)
    //              : "memory");

    // console_write("\nready\n");
    // list_show(&ready_thread_list);
    // console_write("all\n");
    // list_show(&all_thread_list);

    return thread;
}

static void start_main_thread()
{
    main_thread = get_running_thread_pcb(); // get_page(placement_address, 1, kernel_directory);

    init_thread(main_thread, "main-thread", 5);

    assert(!node_find(&all_thread_list, &main_thread->all_tag), "\nrepeat in all thread list");
    list_append(&all_thread_list, &main_thread->all_tag);
}

void thread_init(void)
{
    console_write("\n");
    info;
    console_write("thread init ...");

    list_init(&ready_thread_list);
    list_init(&all_thread_list);

    start_main_thread();

    disk_thread = thread_start("disk-thread", 10, disk, NULL);
    console_write("\n");
    info;
    console_write("thread init done");
}

void schedule()
{
    // list_show(&ready_thread_list);

    assert(interrupt_get_status() == INTR_OFF, "\ninterrupt on may switch!");

    thread_pcb *current_thread = get_running_thread_pcb();

    if (current_thread->status == TASK_RUNNING)
    {

        //只是时间片时限到了，放入就绪队列
        assert(!node_find(&ready_thread_list, &current_thread->tag), "\nrepeat in ready thread list");

        list_append(&ready_thread_list, &current_thread->tag);

        //重置
        current_thread->ticks = current_thread->priority;
        current_thread->status = TASK_READY;
    }
    else
    {
        //因其他原因被阻塞
    }

    // assert(!list_empty(&ready_thread_list), "\nready thread list is empty");
    if (list_empty(&ready_thread_list))
    {
        thread_unblocked(disk_thread);
    }

    thread_tag = NULL;
    thread_tag = list_pop(&ready_thread_list); //弹出第一个就绪线程

    thread_pcb *next = element_entry(thread_pcb, tag, thread_tag); //将tag转换为pcb,以方便具体的数据操作

    // list_show(&ready_thread_list);
    //  console_write_hex(thread_tag,black,white);
    //  console_write_hex(next,black,white);
    //  console_write_hex(current_thread->itself_stack,black,white);

    next->status = TASK_RUNNING;
    switch_thread(current_thread, next);
}

void thread_blocked(enum task_status status)
{
    assert(status == TASK_BLOCKED || status == TASK_DIED || status == TASK_HANGING, "no need for schedule");

    interrupt_status old_status = interrupt_disable();

    thread_pcb *current_thread = get_running_thread_pcb();

    current_thread->status = status;
    schedule();

    interrupt_set_status(old_status);
}

void thread_unblocked(thread_pcb *pthread)
{
    interrupt_status old_status = interrupt_disable();

    assert(pthread->status == TASK_BLOCKED || pthread->status == TASK_DIED || pthread->status == TASK_HANGING, "no need for schedule");

    if (pthread->status != TASK_READY) // wake up!!
    {
        assert(!node_find(&ready_thread_list, &pthread->tag), "\nblocked pthread exist in ready thread list");

        if (node_find(&ready_thread_list, &pthread->tag))
        {
            PANIC("\nblocked pthread exist in ready thread list");
        }

        list_push(&ready_thread_list, &pthread->tag);
        pthread->status = TASK_READY;
    }

    interrupt_set_status(old_status);
}

void thread_yield() // yield(让出)cpu给其他线程
{
    thread_pcb *current_thread = get_running_thread_pcb();

    interrupt_status old_status = interrupt_get_status();

    assert(!node_find(&ready_thread_list, &current_thread->tag), "\nrepeat in ready thread list");
    list_append(&ready_thread_list, &current_thread->tag);

    current_thread->status = TASK_READY; //加入就绪队列
    schedule();

    interrupt_set_status(old_status);
}