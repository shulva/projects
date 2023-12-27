;编写code段的代码，将a段和b段的数据依次相加，将结果存入c段中
;结果为(-d es:0 f) 204F:0000  02 04 06 08 0A 0C 0E 10
assume cs:code
a segment
    db 1,2,3,4,5,6,7,8
a ends

b segment
    db 1,2,3,4,5,6,7,8
b ends

c segment
    db 0,0,0,0,0,0,0,0
c ends

code segment
    start:              ;补全start部分
        mov ax,a
        mov ds,ax
        mov bx,b
        mov ss,bx
        mov cx,c
        mov es,cx

        mov bx,0
        mov cx,8

    s:
        mov ax,ds:[bx]
        add ax,ss:[bx]
        mov es:[bx],ax
        inc bx
        loop s

        mov ax,4c00h
        int 21h

code ends
end start