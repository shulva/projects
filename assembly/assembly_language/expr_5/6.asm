;程序如下，编写code段的代码，用Push指令将a段中的前8个字型数据，逆序存储到b段中
;答案如下: (-d ss:0 f) 204F:0000  08 00 07 00 06 00 05 00-04 00 03 00 02 00 01 00
assume cs:code
a segment
    dw 1,2,3,4,5,6,7,8,9,0ah,0bh,0ch,0dh,0eh,0fh,0ffh
a ends

b segment
    dw 0,0,0,0,0,0,0,0
b ends

code segment
    start:              ;补全start

        mov ax,a
        mov es,ax

        mov bx,b
        mov ss,bx

        mov sp,0010h

        mov bx,0
        mov cx,8

    s:
        push es:[bx]
        inc bx
        inc bx
        loop s

        mov ax,4c00h
        int 21h

code ends
end start