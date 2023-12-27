;Q: cpu执行程序，程序返回前，data段中的数据为？
;A: 204D:0000  23 01 DF 0B BC 0A EF 0D-ED 0F BA 0C 87 09 00 00 
;Q: cpu执行程序，程序返回前，cs= ,ss= ,ds= 
;A: DS=204D  SS=204E  CS=204F
;Q: 设程序加载后，code段的段地址为x,则data段的段地址为，stack的段地址为
;A: 可知 DS=204D  ES=203D  SS=204E  CS=204F,即 data=x-2,stack=x-1
assume cs:code,ds:data,ss:stack

data segment
         dw 0123h,0456h,0789h,0abch,0defh,0fedh,0cbah,0987h
data ends

stack segment
          dw 0,0,0,0,0,0,0,0
stack ends

code segment
    start:
          mov  ax,stack
          mov  ss,ax
          mov  sp,16

          mov  ax,data
          mov  ds,ax

          push ds:[0]
          push ds:[2]
          pop  ds:[2]
          pop  ds:[0]

          mov  ax,4c00h
          int  21h
code ends

end start
