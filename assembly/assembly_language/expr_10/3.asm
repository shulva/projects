;数值显示
;编程，将data段中的数据以十进制的形式显示出来
;
;dtoc
;将word型数据转变为表示十进制数的字符串，字符串以0为结尾符
;
;参数
;ax=word型数据
;ds:si指向字符串的首地址
;
;返回：无
;
;相关提示请去电子书上找，太多了
;jsdos masm-v5.00没有问题
;jsdos masm-v6.11会有非常奇怪的问题
assume cs:code,ds:data,ss:stack
data segment
         db 10 dup(0)
data ends

stack segment
          dw 8 dup(0)
stack ends

code segment
    start:   
             mov  ax,12345            ;8位二进制,10进制最多也就5位

             mov  bx,data
             mov  ds,bx
             mov  si,4

             call dtoc

             mov  dh,8
             mov  dl,3
             mov  cl,2

             call show_str

             mov  ax,4c00h
             int  21h

    dtoc:    
             mov  dx,0000h            ;做16位除法是因为完全可能发生除法溢出现象
             mov  bx,10
             div  bx

             add  dl,30h              ;余数
             mov  ds:[si],dl
             mov  cx,ax

             dec  si                  ;si自动调节开始的位置

             inc  cx
             loop dtoc

             inc  si
             ret
   
    show_str:
             mov  ax,0b800h
             mov  es,ax

             mov  al,00a0h            ;行距
             mul  dh
             mov  bx,ax

             mov  dh,0
             mov  di,dx               ;dl 列距

             mov  dx,stack
             mov  ss,dx
             mov  sp,0010h

             mov  ch,0
             mov  dl,cl

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
