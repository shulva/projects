;分析下面的程序，在分析前思考：这个程序可以正确返回吗?
;运行后再思考，为何是这种结果
;A:我一开始是以为不能正确返回的，因为最后还是jmp short s1从而无限循环
;A:后来运行后我明白了，jmp short s1本身存入的是相对的改变ip的操作，在s标号处改变ip到mov ax,4c00h的长度与从s2标号处跳转到s1是一样的三条指令
;A:故而在s:处的jmp short s1实际上是转移到了mov ax,4c00h处，从而可以正常返回
assume cs:codesg
codesg segment

              mov ax,4c00h
              int 21h

       start: mov ax,0
       s:     nop
              nop

              mov di,offset s
              mov si,offset s2
              mov ax,cs:[si]         ; jmp short s1
              mov cs:[di],ax         ; replace nop nop

       s0:    jmp short s

       s1:    mov ax,0
              int 21h
              mov ax,0

       s2:    jmp short s1
              nop

codesg ends
end start