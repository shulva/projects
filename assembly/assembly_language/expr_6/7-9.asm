;(1)太多了，不想做
;(2)即是问题7.9
assume cs:codesg,ds:datasg,ss:stacksg

stacksg segment
  dw 0,0,0,0,0,0,0,0
stacksg ends

datasg segment
    db '1. display      ' ; 16位
    db '2. brows        ' ; 16位
    db '3. replace      ' ; 16位
    db '4. modify       ' ; 16位
datasg ends

codesg segment
  start:
      mov ax,datasg
      mov ds,ax
      mov bx,0000h

      mov ax,stacksg
      mov ss,ax
      mov sp,0010h

      mov ax,0
      mov cx,4

    s0:
      push cx
      mov si,3
      mov cx,4
    s1:
      mov al,ds:[bx+si]
      and al,11011111b
      mov ds:[bx+si],al
      inc si
      loop s1

      add bx,0010h
      pop cx

      loop s0

      mov ax,4c00h
      int 21h
codesg ends

end start