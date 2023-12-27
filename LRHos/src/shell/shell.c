#include "../header/shell.h"
#include "../header/ring_buffer.h"
#include "../header/string.h"
#include "../header/thread.h"
#define cmd_len 64

static char cmd_line[cmd_len] = {0};
static int position = 0;
static void ps(double_linked_list_node *node, int arg);

void print_prompt(void)
{
    console_write("[LRH@localhost]#");
}

void myshell(ring_buffer *buffer)
{
    char current = getchar_from_buffer(buffer);
    // console_put_color(current, black, white);

    cmd_line[position] = current;

    switch (current)
    {
        /* 找到回车或换行符后认为键入的命令结束,直接返回 */
    case '\n':
    case '\r':
        current = 0; // 添加cmd_line的终止字符0
        shell_interpret();
        return;

    case '\b':
        if (cmd_line[0] != '\b') // 阻止删除非本次输入的信息
        {
            --position; // 退回到缓冲区cmd_line中上一个字符
            console_put_color('\b', black, white);
        }
        break;

    /* 非控制键则输出字符 */
    default:
        console_put_color(current, black, white);
        position++;
    }
}

void shell_interpret()
{
    char *buffer = kmalloc(64);

    for (int i = 0; i < position; i++)
    {
        buffer[i] = cmd_line[i];
    }

    position = 0;

    if (0 == strcmp("clear", buffer))
    {
        console_clear();
    }
    else if (0 == strcmp("ps", buffer))
    {
        console_write("\n");
        console_write("name");
        console_write("\t");
        console_write_color("priority", black, white);
        console_write("  ");
        console_write_color("done_ticks", black, white);

        list_traversal(&all_thread_list, ps, NULL);
        console_put_color('\n', black, white);
    }
    else if (0 == strcmp("block", buffer))
    {
        console_write("\n");
        thread_blocked(TASK_BLOCKED);
    }
    else
    {
        console_put_color('\n', black, white);
        print_prompt();
        return;
    }

    print_prompt();
}

void ps(double_linked_list_node *node, int arg)
{
    thread_pcb *current = element_entry(thread_pcb, all_tag, node);

    console_write("\n");
    // console_write_dec(current->pid, black, white);
    // console_write("\t");
    console_write(current->name);
    console_write("\t");
    console_write_dec(current->priority, black, white);
    console_write("\t   ");
    console_write_dec(current->done_ticks, black, white);
}