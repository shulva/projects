;实验材料较多，请去电子书处查看
;在屏幕中间分别显示闪烁绿色,绿底红色,白底蓝字的'welcome to masm!'
;jsdos 可行
;从屏幕中间的10行，30列开始显示
assume cs:code,ds:data,ss:stack
data segment
           db 'welcome to masm!'                 ;16个字节
           db 10000010b,00100100b,01110001b      ;颜色
data ends

stack segment
            dw 8 dup(0)
stack ends

code segment
      start:
            mov  ax,0b800h
            mov  es,ax                 ;第10行偏移为0640h,两行之间偏移差距为00a0h
            mov  bx,0640h
            mov  ax,0
          
            mov  dx,stack              ;栈
            mov  ss,dx
            mov  sp,0010h
        
            mov  dx,data               ;数据段
            mov  ds,dx

            mov  bp,0                  ;color的index
            mov  cx,3
          
      s:    
            push cx
            mov  cx,16
            mov  si,0                  ;string的index
            mov  di,0
      w:    
            mov  al,ds:[si]
            mov  ah,ds:[0010h+bp]
            mov  es:[bx+di+60],ax      ;60-61对应第30列&显存的index

            inc  si
            add  di,2
            loop w

            add  bx,00a0h              ;换到下一行
            pop  cx
            inc  bp
            loop s

            mov  ax,4c00h
            int  21h

code ends
end start