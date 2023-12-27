;(1) 编程，向内存0:0200~0:023f依次传输数据0~63(3fh)
;需要使用jsdos,msdos这段内存空间有别的内容
assume cs:code

code segment
          mov  ax,0000h
          mov  ds,ax

          mov  bx,0200h
          mov  cx,64

          mov  dx,0

     s:   mov  ds:[bx],dl
          inc  bx
          inc  dx
          loop s

          mov  ax,4c00h
          int  21h
    
code ends
end