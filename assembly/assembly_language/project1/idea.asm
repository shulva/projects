;课程设计1
;具体材料请去电子书上看
;dh为行号(0-24),dl为列号(0-79),cl=颜色，ds;si指向字符串的首地址
;
; dtoc
; 将dword型数据转变为表示十进制数的字符串，字符串以0为结尾
; 
; 参数:
; ax=dword的低16位
; da=dword的高16位
; ds:si指向字符串首地址

; 返回:无
;
; divdw
; 进行不会产生溢出的除法运算(eg:1000/1=1000,会产生8位的溢出)，被除数为dword,除数为word,结果为dword
;
; 参数：
; ax=dword数据的低16位，dx=dword数据的高16位，cx=除数
; 返回:
; dx=结果的高16位，ax=结果的低16位
; cx=余数

assume cs:code,ds:data,ss:stack
stack segment
           dw 1024 dup(0)
stack ends

data segment
     ;21年份
          db '1975', '1976', '1977', '1978', '1979', '1980', '1981', '1982', '1983'
          db '1984', '1985', '1986', '1987', '1988', '1989', '1990', '1991', '1992'
          db '1993', '1994', '1995'

     ;21年收入
          dd 16, 22, 382, 1356, 2390, 8000, 16000, 24486, 50065, 97479, 140417, 197514
          dd 345980, 590827, 803530, 1183000, 1843000, 2759000, 3753000, 4649000, 5937000

     ;21年员工数目
          dw 3, 7, 9, 13, 28, 38, 130, 220, 476, 778, 1001, 1442, 2258, 2793, 4037, 5635, 8226
          dw 11542, 14430, 15257, 17800
data ends

code segment
     start:     
                mov  ax,0b800h
                mov  es,ax
                mov  cx,4000
                
                mov  bp,0
     clear:     
                mov  byte ptr es:[bp],32
                add  bp,2
                loop clear


                mov  dx,stack
                mov  ss,dx
                mov  sp,0400h

                mov  bx,0                    ;计数器

                mov  ax,data
                mov  ds,ax

                mov  si,4

                mov  cx,4

     dowhile:                                ;循环1

                dec  si                      ;防越界

                mov  dl,ds:[si]
                mov  dh,0
                push dx

                add  bx,1

                mov  ax,cx
                mov  cx,si
                sub  cx,80
                jcxz go
                mov  cx,ax

                loop dowhile

                mov  dx,'|'
                push dx
                add  bx,1

                add  si,8

                mov  cx,4
                jmp  dowhile
             
     go:        
                add  si,4
                mov  dx,'|'
                push dx
                add  bx,1

     dtoc:                                   ;转化为字符串
                
                mov  ax,ds:[si]
                mov  dx,ds:[si+2]


     for:       mov  cx,10                   ;循环2

                call divdw
                add  cx,30h
                mov  ch,0
                push cx                      ;push按字操作
                add  bx,1

                sub  cx,cx
                add  cx,ax
                add  cx,dx
               
                inc  cx
                loop for                     ;除的结果为0

                mov  cx,'|'
                push cx
                add  bx,1

                add  si,4
            
                mov  cx,si
                sub  cx,167
                loop dtoc

     wtoc:      
                mov  ax,ds:[si]
                mov  dx,0                    ;做32位除法，因为其会有除法溢出问题

     while:                                  ;循环3
                mov  cx,10
                div  cx

                mov  cx,ax                   ;ax是商，dx是余数

                add  dx,30h
                push dx
             
                add  bx,1
                mov  dx,0
                inc  cx

                loop while

                add  si,2

                mov  cx,'|'
                push cx
                add  bx,1

                mov  cx,si
                sub  cx,209
                loop wtoc

                mov  di,84
                mov  bp,168

     divfor_avr:                             ;循环4,第4列
                mov  ax,ds:[di]
                mov  dx,ds:[di+2]
                mov  cx,ds:[bp]

                call divdw

     looop:     mov  cx,10                   ;有溢出问题
                div  cx
      
                mov  cx,ax

                add  dx,30h                  ;ax为商，dx是余数
                push dx

                add  bx,1
                mov  dx,0
                inc  cx
                loop looop
               
                mov  cx,'|'
                push cx
                add  bx,1

                add  di,4
                add  bp,2

                mov  cx,di
                sub  cx,167

                loop divfor_avr

     ;字符串转化全部完成
     ;按字符矩阵的右下角落来定位
     
                mov  dh,21                   ;行(共25行)
                mov  dl,66                   ;列(共80列)
                mov  cl,7

                call show_str

                mov  ax,4c00h
                int  21h

     divdw:                                  ; 除法程序
                push bx
                push ax

                mov  ax,dx
                mov  dx,0000h
          
                div  cx
                
                mov  bx,ax                   ;ax=int()

                pop  ax                      ;L
                div  cx                      ;ax为商

                mov  cx,dx                   ;余数
                mov  dx,bx                   ;高16位

                pop  bx
                ret
     show_str:                               ;显示程序
                mov  ax,0b800h
                mov  es,ax

                mov  al,160
                mul  dh
                mov  bp,ax                   ;行距

                mov  al,2
                mul  dl
                mov  dx,ax
                mov  dh,0
                mov  di,dx                   ;列距

                mov  ch,0
                mov  si,di
                
                mov  dh,cl
                mov  cx,bx
                mov  bl,dh

                mov  bh,0                    ;行计数器
                
                pop  dx                      ;keep it simple and stupid.. haha
                pop  dx
                dec  cx

     draw:      

                pop  dx

                push cx
                mov  dh,0
                mov  cx,dx
                sub  cx,'|'
                jcxz border
                pop  cx

                mov  al,dl
                mov  ah,bl
                mov  es:[bp+di], ax          ;bx->行,di->列

                add  di,2

                loop draw
                ret
     border:    
                pop  cx
                sub  bp,160
                
                mov  di,si

                add  bh,1

                push cx
                mov  ch,0
                mov  cl,bh

                sub  cx,21
                jcxz column
                pop  cx

                loop draw                    ;自动cx-1

     column:    
                pop  cx
                mov  bh,0

                sub  si,40                   ;列之间的距离
                mov  di,si
                
                add  bp,0d20h                ;0d20=00a0*21
               
                loop draw
                
code ends
end start