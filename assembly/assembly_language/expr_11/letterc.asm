;letterc
;将以0结尾的字符串中的小写字母转变为大写字母
;ds:si指向字符串首地址

assume cs:codesg

datasg segment
    db "Beginner's All-purpose symbolic Instruction Code.",0
datasg ends

codesg segment
    begin:
        mov ax,datasg
        mov ds,ax
        mov si,0
        call letterc

        mov ax,4c00h
        int 21h

    letterc:
        mov al,ds:[si]
        cmp al,61h      ;61h 'a',7ah 'z'
        jb remake
        cmp al,7ah
        ja remake

        sub al,20h
        mov ds:[si],al

    remake: 
        inc si
        mov bl,ds:[si]
        mov bh,00h
        cmp bl,bh
        jne letterc
        ret 

codesg ends
end begin