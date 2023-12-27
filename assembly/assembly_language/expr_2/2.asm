code segment                      ;use -t to debug  debug -t 在执行修改ss寄存器的命令后会紧接着执行下一条命令，故有时看不到有些命令被执行了
            assume cs:code
      start:
            mov    ax,65535
            mov    ds,ax
            mov    ax,2200
            mov    ss,ax
            mov    sp,0100
            mov    ax,ds:[0]
            add    ax,ds:[2]
            mov    bx,ds:[4]
            add    bx,ds:[6]
            push   ax
            push   bx
            pop    ax
            pop    bx
            push   ds:[4]
            push   ds:[6]
code ends
end start