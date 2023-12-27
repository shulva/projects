#include "../header/port.h"
/*
asm ( assembler template : output operands    optional
			 : input operands                 optional
			 : list of clobbered registers    optional
	);

outb format -> outb reg,byte
inb  format -> inb  byte,port
inw  format -> inw  word,port
volatile 禁止优化，让其必须在被放置的地方执行
参数:
"a" 寄存器操作数constraints %eax, %ax, %al
"=" 指明这个操作数是只写的,之前保存在其中的值将被废弃而被输出值所代替
"d" 寄存器操作数constraints %edx, %dx, %dl
"N" Constant in range 0 to 255 (for out instruction)
*/

/* 输入操作数 */
inline void outbyte(uint16_t port, uint8_t value)
{
	asm volatile("outb %1,%0"
				 :
				 : "dN"(port), "a"(value));
}

/* 输出操作数 */
inline uint8_t inbyte(uint16_t port)
{
	uint8_t ret;

	asm volatile("inb %1,%0"
				 : "=a"(ret)
				 : "dN"(port));

	return ret;
}

inline uint16_t inword(uint16_t port)
{
	uint16_t ret;

	asm volatile("inw %1,%0"
				 : "=a"(ret)
				 : "dN"(port));

	return ret;
}

inline void outsw(uint16_t port, const void *addr, uint32_t num)
{
	/*********************************************************
	   +表示此限制即做输入又做输出.
	   outsw是把ds:esi处的16位的内容写入port端口, 我们在设置段描述符时,
	   已经将ds,es,ss段的选择子都设置为相同的值了,此时不用担心数据错乱。*/
	asm volatile("cld; rep outsw"
				 : "+S"(addr), "+c"(num)
				 : "d"(port)
				 : "memory"); //: "memory"是后加的，必要时可以考虑删除
							  /******************************************************/
}

inline void insw(uint16_t port, void *addr, uint32_t num)
{
	/******************************************************
	   insw是将从端口port处读入的16位内容写入es:edi指向的内存,
	   我们在设置段描述符时, 已经将ds,es,ss段的选择子都设置为相同的值了,
	   此时不用担心数据错乱。*/
	asm volatile("cld; rep insw"
				 : "+D"(addr), "+c"(num)
				 : "d"(port)
				 : "memory");
	/******************************************************/
}
