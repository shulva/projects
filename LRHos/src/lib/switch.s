[bits 32]
section .text
global switch_thread
switch_thread:
    ;thread_stack的结构(栈的地址是自高向低扩展的)
    push esi 
    push edi
    push ebx
    push ebp

    ;p433 有图，很容易理解
    mov eax,[esp+20];用eax保存当前线程current_thread的栈地址
    mov [eax],esp   ;将当前栈顶指针esp保存到当前线程current_thread的itself_stack中

    ;保存当前线程环境

    ;恢复next线程环境
    mov eax,[esp+24];用eax保存下个线程next的栈地址
    mov esp,[eax]   ;将next的栈指针放到esp中

    pop ebp
    pop ebx
    pop edi
    pop esi

    ret


