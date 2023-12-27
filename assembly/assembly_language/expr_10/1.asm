;此次实验会编写3个子程序
;我们会通过他们来认识几个常见的问题并掌握解决这些问题的方法
; 
;1-显示字符串
;显示字符串，编写通用子程序，使调用者可以决定显示的内容，位置及颜色
;dh为行号(0-24),dl为列号(0-79),cl=颜色，ds;si指向字符串的首地址

;子程序的入口参数是屏幕上的行号与列号

assume cs:code,ds:data,ss:stack
data segment
         db 'welcome to masm!',0    ;通过循环读0判断是否结束
data ends
stack segment
          dw 8 dup(0)
stack ends

code segment
    start:   
             mov  dh,8
             mov  dl,3
             mov  cl,2
             mov  ax,data
             mov  ds,ax
             mov  si,0
             
             call show_str

             mov  ax,4c00h
             int  21h

    show_str:
             mov  ax,0b800h
             mov  es,ax

             mov  al,00a0h            ;行距
             mul  dh
             mov  bx,ax

             mov  dh,0
             mov  di,dx

             mov  dx,stack
             mov  ss,dx
             mov  sp,0010h

             mov  ch,0
             mov  dl,cl
             mov  si,0

    draw:    
             mov  cl,dl
             mov  al,ds:[si]
             mov  ah,cl
             mov  es:[bx+di+1], ax

             inc  si
             add  di,2

             mov  cl,ds:[si]
             add  cl,1                ;loop 会自动对cx-1

             loop draw
             ret

code ends
end start