[GLOBAL gdt_flush]
gdt_flush:
    mov eax, [esp+4]  ; 参数存入 eax 寄存器
    lgdt [eax]        ; 加载到 GDTR 

    mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment(index=2)
    mov ds, ax        ; 更新所有可以更新的段寄存器,对应了在port.c中将段选择符设置为同一值的注释
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:flush   ; 远跳转， 0x08 = 0b00001000 , 1是index位，即cs
    
flush:
    ret