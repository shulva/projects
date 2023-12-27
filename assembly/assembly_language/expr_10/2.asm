; divdw
; 进行不会产生溢出的除法运算(eg:1000/1=1000,会产生8位的溢出)，被除数为dword,除数为word,结果为dword
;
; 参数：
; ax=dword数据的低16位，dx=dword数据的高16位，cx=余数
; 返回:
; dx=结果的高16位，ax=结果的低16位
; cx=余数
;
; eg: 1000000/10(f4240h/0ah)
; mov ax,4240h
; mov dx,000fh
; mov cx,0ah
; call divdw
;
; 结果: dx=0001h,ax=86a0h,cx=0
;
; hint: 给出一个公式: x/n=int(h/n)*65536+[rem(h/n)*65536+L]/n
;
; x:被除数，0~ffffffff
; n:除数，0~ffff
; h:x高16位
; L:x低16位
; int():描述性运算符，取商，比如int(38/3)=3
; rem():描述性运算符，取余数，比如rem(38/10)=8
; 这个公式将可能产生溢出的除法运算:x/n,转化为多个不会产生溢出的除法运算
; 公式中，等号右边所有的除法运算都可以用div指令，肯定不会导致除法溢出
assume cs:code,ss:stack
stack segment
            dw 8 dup(0)
stack ends

code segment
      start:
            mov  bx,stack
            mov  ss,bx
            mov  sp,0010h

            mov  dx,0000fh      ;高位
            mov  ax,4240h       ;低位
            mov  cx,000ah       ;除数
        
            sub  bx,bx
            call divdw
            mov  ax,4c00h
            int  21h

      divdw:
            push ax

            mov  ax,dx
            mov  dx,0000h
          
            div  cx

            mov  bx,ax          ;ax=int()

            pop  ax             ;L
            div  cx             ;ax为商

            mov  cx,dx          ;余数
            mov  dx,bx          ;高16位
            ret
code ends
end start