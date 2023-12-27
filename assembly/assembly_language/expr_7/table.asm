; data段和table段都是预先给出的
;
;
;
;
;
assume cs:codesg
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

table segment
             db 21 dup ('year sumn ne ?? ')
table ends


codesg segment

       start: 
              mov  ax,data
              mov  ds,ax

              mov  ax,table
              mov  es,ax

              mov  bx,0000h
  
              mov  cx,21
              mov  si,0

       year:  
              mov  ax,ds:[si]
              mov  es:[bx],ax
              add  si,2
              mov  ax,ds:[si]
              mov  es:[bx+2],ax
              add  si,2
              mov  byte ptr es:[bx+4],' '
              add  bx,0010h
              loop year

              mov  cx,21
              mov  bx,0000h

       income:
              mov  ax,ds:[si]
              mov  es:[bx+5],ax
              add  si,2
              mov  ax,ds:[si]
              mov  es:[bx+7],ax
              add  si,2
              mov  byte ptr es:[bx+9],' '
              add  bx,0010h
              loop income

              mov  cx,21
              mov  bx,0000h

       num:   
              mov  ax,ds:[si]
              mov  es:[bx+10],ax
              add  si,2
              mov  byte ptr es:[bx+12],' '
              add  bx,0010h
              loop num

              mov  cx,21
              mov  bx,0000h

       per:   
              mov  dx,es:[bx+7]
              mov  ax,es:[bx+5]
              div  word ptr es:[bx+10]
              mov  es:[bx+13],ax
              mov  byte ptr es:[bx+15],' '
              add  bx,0010h
              loop per

              mov  ax,4c00h
              int  21h
    
codesg ends
end start