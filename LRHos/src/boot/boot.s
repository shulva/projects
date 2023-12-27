;-------------------
;
;boot.s
;
;-------------------
;
;multiboot 规范可以上网找，很多
;multiboot magic num 
MBOOT_HEADER_MAGIC equ 0x1BADB002

; 0 号位表示所有的引导模块将按页(4KB)边界对齐
MBOOT_PAGE_ALIGN    equ     1 << 0

; 1 号位通过 Multiboot 信息结构的 mem_* 域包括可用内存的信息
; (告诉GRUB把内存空间的信息包含在Multiboot信息结构中)
MBOOT_MEM_INFO      equ     1 << 1    

; 定义我们使用的 Multiboot 的标记
MBOOT_HEADER_FLAGS  equ     MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO

; 域checksum是一个32位的无符号值，当与其他的magic域(也就是magic和flags)
; 相加时，要求其结果必须是32位的无符号值 0 (即magic+flags+checksum = 0)
MBOOT_CHECKSUM      equ     -(MBOOT_HEADER_MAGIC+MBOOT_HEADER_FLAGS)

[BITS 32]   ;所以代码以 32-bit 的方式编译
section .text

dd MBOOT_HEADER_MAGIC   ; GRUB 会通过这个魔数判断该映像是否支持grub
dd MBOOT_HEADER_FLAGS   ; How GRUB should load your file / settings 
dd MBOOT_CHECKSUM       ; To ensure that the above values are correct

[GLOBAL start]      	; 向外部声明内核代码入口，此处提供该声明给链接器
[GLOBAL glb_mboot_ptr]  ; 向外部声明 struct multiboot * 变量
[EXTERN kern_entry]     ; 声明内核 C 代码的入口函数

start:
	cli		;关中断，没有中断向量表，没有中断处理
	mov esp,STACK_TOP
	mov ebp,0
	and esp,0FFFFFFF0H
	mov [glb_mboot_ptr],ebx ;On bootup, GRUB will load a pointer to another information structure into the EBX register
							;so we save it in this global variable

	call kern_entry



stop:
	hlt
	jmp stop	;无限循环，先卡住，关机后面再写

;-----------------------

section .bss
stack:
	resb 32768

glb_mboot_ptr:
	resb 4

STACK_TOP equ $-stack-1 


	
