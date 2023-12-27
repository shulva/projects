;(3) 下面的程序的功能是将 mov ax,4c00h 之前的指令复制到内存0:0200处，补全程序，上机调试
;hint1:复制的是什么？从哪里到哪里?
;hint2:复制的是什么?有多少个字节?你如何知道要复制的字节的数量？
assume cs:code

code segment
         mov  ax,cs         ;补全源操作数
         mov  ds,ax

         mov  ax,0020h
         mov  es,ax
         mov  bx,0
         mov  cx,0017h      ;补全源操作数 ..cx乃程序长度，或许你应该随便输入一个数，然后在debug里看一下有多大,我这里是0017h

    s:   mov  al,[bx]
         mov  es:[bx],al
         inc  bx
         loop s

         mov  ax,4c00h
         int  21h
    
code ends
end