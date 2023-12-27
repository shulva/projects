# makefile for os
# for c & c++ rules
#
# shell 执行shell命令
# $(patsubst 原模式， 目标模式， 文件列表)
# $@  表示目标文件
# $<  表示第一个依赖文件
# %.o 表示所有以o结尾的目标
# .c.o 即是 %.o : %.c
# 即所有的.o文件，依赖于对应的.c文件
# 类似于a.o:a.c,b.o:b.c
# .PHONY 可以防止可执行命令的目标规则与实际文件出现冲突
# $(RM)=rm -f
#
# C_FLAGS: -m32 32位代码 -ggdb -gstabs+ 添加调试信息 -nostdinc  不包含c语言标准库里的头文件 -fno-builtin gcc不主动使用自己的内建函数 -fno-stack-protector 不使用栈保护等检测
# LD_FLAGS -t 使用自己的链接器脚本 -m elf_i386 生成i386平台下的elf格式的可执行文件(linux下的格式) -nostdlib 不链接c语言的标准库
#
c_source = $(shell find . -name "*.c")
c_obj = $(patsubst %.c,%.o,$(c_source))
s_source = $(shell find . -name "*.s")
s_obj = $(patsubst %.s,%.o, $(s_source))

CC = gcc
LD = ld
ASM = nasm

C_FLAGS = -c -Wall -m32 -ggdb -gstabs+ -nostdinc -fno-pic -fno-builtin -fno-stack-protector -I include
LD_FLAGS = -T scripts/Link.ld -m elf_i386 -nostdlib
ASM_FLAGS = -f elf -g -F stabs


all :$(s_obj) $(c_obj) link update
 
.c.o:
	@echo compiling $< ...
	$(CC) $(C_FLAGS) $< -o $@

.s.o: 
	@echo asming $< ...
	$(ASM) $(ASM_FLAGS) $<

link: 
	@echo linking ...
	$(LD) $(LD_FLAGS) $(s_obj) $(c_obj) -o kernel

.PHONY:clean
clean:
	$(RM) $(s_obj) $(c_obj) kernel

.PHONY:update
update:
	sudo mount floppy.img /mnt/kernel
	sudo cp kernel /mnt/kernel/kernel
	sudo umount /mnt/kernel 	

.PHONY:mount
mount:
	sudo mount floppy.img /mnt/kernel

.PHONY:umount
umount:
	sudo umount /mnt/kernel

.PHONY:qemu
qemu:
	qemu -fda floppy.img -hda hd80.img -boot a 

.PHONY:debug
debug:
	qemu -S -s -fda floppy.img -hda hd80.img -boot a &
	cgdb -x scripts/gdbinit
