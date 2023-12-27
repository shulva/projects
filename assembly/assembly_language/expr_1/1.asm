code segment                     ;use -t to debug
            assume cs:code
      start:
            mov    ax,4e20h
            add    ax,1416h
            mov    bx,2000h
            add    ax,bx
            mov    bx,ax
            add    ax,bx
            mov    ax,001ah
            mov    bx,0026h
            add    al,bl
            add    ah,bl
            add    bh,al
            mov    ah,0
            add    al,bl
            add    al,9ch
code ends
end start