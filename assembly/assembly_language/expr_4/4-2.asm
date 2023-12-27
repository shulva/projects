;(2) 编程，向内存0:0200~0:023f依次传输数据0~63(3fh),只能使用九条指令，包括int 21h与mov ax,4c00h
;用bx代替原先dx的功能就行了
;需要使用jsdos,msdos这段内存空间有别的内容
assume cs:code

code segment
         mov  ax,0020h
         mov  ds,ax

         mov  bx,0000h
         mov  cx,64


    s:   mov  ds:[bx],bl    ;(注意内存单元大小的关系)
         inc  bx
         loop s

         mov  ax,4c00h
         int  21h
    
code ends
end