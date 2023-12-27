;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;ISR
%macro isr_no_error 1
[GLOBAL isr%1]
isr%1:
    cli                  ; 关中断
    push 0               ; push0作为无效中断号，从而避免对不自动压入错误号的中断处理函数的分类处理
    push %1              ; push 中断号

    jmp isr_protect
%endmacro

; 用于有错误代码的中断
%macro isr_error 1
[GLOBAL isr%1]
isr%1:
    cli                  ; 关中断
    push %1    
      
    jmp isr_protect
%endmacro


isr_no_error  0    ; 0 #DE 除 0 异常
isr_no_error  1    ; 1 #DB 调试异常
isr_no_error  2    ; 2 NMI
isr_no_error  3    ; 3 BP 断点异常 
isr_no_error  4    ; 4 #OF 溢出 
isr_no_error  5    ; 5 #BR 对数组的引用超出边界 
isr_no_error  6    ; 6 #UD 无效或未定义的操作码 
isr_no_error  7    ; 7 #NM 设备不可用(无数学协处理器) 
isr_error     8    ; 8 #DF 双重故障(有错误代码) 
isr_no_error  9    ; 9 协处理器跨段操作
isr_error    10    ; 10 #TS 无效TSS(有错误代码) 
isr_error    11    ; 11 #NP 段不存在(有错误代码) 
isr_error    12    ; 12 #SS 栈错误(有错误代码) 
isr_error    13    ; 13 #GP 常规保护(有错误代码) 
isr_error    14    ; 14 #PF 页故障(有错误代码) 
isr_no_error 15    ; 15 CPU 保留 
isr_no_error 16    ; 16 #MF 浮点处理单元错误 
isr_error    17    ; 17 #AC 对齐检查 
isr_no_error 18    ; 18 #MC 机器检查 
isr_no_error 19    ; 19 #XM SIMD(单指令多数据)浮点异常

; 20 ~ 31 Intel 保留
isr_no_error 20
isr_no_error 21
isr_no_error 22
isr_no_error 23
isr_no_error 24
isr_no_error 25
isr_no_error 26
isr_no_error 27
isr_no_error 28
isr_no_error 29
isr_no_error 30
isr_no_error 31
; 32 ～ 255 用户自定义
isr_no_error 255

;保护现场
[EXTERN isr_handle]
isr_protect:

    push ds
    push es
    push fs
    push gs

    pusha                    ; Pushes edi, esi, ebp, esp, ebx, edx, ecx, eax

    
    push esp                ; 此时的 esp 寄存器的值等价于 isr_reg 结构体的指针
    call isr_handle         ; idt.c
    add esp, 4              ; 清除压入的参数
    
    popa                     ; Pops edi, esi, ebp, esp, ebx, edx, ecx, eax

    pop gs
    pop fs
    pop es
    pop ds
    
    add esp, 8               ; 清理栈里的 error code 和 int_num
    sti                      ; 开中断
    iret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;IRQ

%macro IRQ 2
[GLOBAL irq%1]
irq%1:
    cli
    push byte 0
    push byte %2
    jmp irq_protect
%endmacro

IRQ   0,    32  ; 电脑系统计时器
IRQ   1,    33  ; 键盘
IRQ   2,    34  ; 与 IRQ9 相接，MPU-401 MD 使用
IRQ   3,    35  ; 串口设备
IRQ   4,    36  ; 串口设备
IRQ   5,    37  ; 建议声卡使用
IRQ   6,    38  ; 软驱传输控制使用
IRQ   7,    39  ; 打印机传输控制使用
IRQ   8,    40  ; 即时时钟
IRQ   9,    41  ; 与 IRQ2 相接，可设定给其他硬件
IRQ  10,    42  ; 建议网卡使用
IRQ  11,    43  ; 建议 AGP 显卡使用
IRQ  12,    44  ; 接 PS/2 鼠标，也可设定给其他硬件
IRQ  13,    45  ; 协处理器使用
IRQ  14,    46  ; IDE0 传输控制使用
IRQ  15,    47  ; IDE1 传输控制使用

[GLOBAL irq_protect]
[EXTERN irq_handle]
irq_protect:

    push ds
    push es
    push fs
    push gs

    pusha                    ; Pushes edi, esi, ebp, esp, ebx, edx, ecx, eax
    
    push esp
    call irq_handle
    add esp, 4
    
    popa                     ; Pops edi, esi, ebp, esp, ebx, edx, ecx, eax

    pop gs
    pop fs
    pop es
    pop ds
    
    add esp, 8           ; 清理压栈的 错误代码 和 int_num 编号
    sti
    iret                 ; 出栈 CS, EIP, EFLAGS, SS, ESP
