#include "../header/console.h"

//汇编经典显存起始位置
static uint16_t *vram = (uint16_t *)0xB8000;
//光标的坐标
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

static void move_cursor()
{
	// 80column,25row
	uint16_t location = cursor_y * 80 + cursor_x;

	// we send thr controller's command port (0x3d4) the command 14 to tell it we are sending the high byte
	// then send that byte to port 0x3D5
	//  We then repeat with the low byte, but send the command 15 instead
	// 8位的限制，没办法
	outbyte(0x3d4, 14);
	outbyte(0x3d5, location >> 8);

	outbyte(0x3d4, 15);
	outbyte(0x3d5, location);
}

static void scroll()
{
	uint8_t backColour = 0;
	uint8_t foreColour = 15;

	//看不懂请自行学习textmode的显存知识
	uint8_t attributeByte = (backColour << 4) | (foreColour & 0x0F);
	uint16_t blank = 0x20 | (attributeByte << 8); // ascii_space=0x20

	if (cursor_y >= 25)
	{
		int i;
		//将所有的复制到上一行
		for (i = 0; i < 24 * 80; i++)
		{
			vram[i] = vram[i + 80];
		}
		//最后一行拿空格
		for (i = 24 * 80; i < 25 * 80; i++)
		{
			vram[i] = blank;
		}

		cursor_y = 24;
	}
}

void console_clear()
{
	uint8_t back_color = 0;
	uint8_t fore_color = 15;
	uint8_t attributeByte = (back_color << 4) | (fore_color & 0x0F);
	uint16_t blank = 0x20 | (attributeByte << 8);

	int i;
	for (i = 0; i < 80 * 25; i++)
	{
		vram[i] = blank;
	}

	cursor_x = 0;
	cursor_y = 0;
	move_cursor();
}

void console_put_color(char c, color back_color, color fore_color)
{

	console_acquire_lock();

	uint8_t back = (uint8_t)back_color;
	uint8_t fore = (uint8_t)fore_color;

	uint8_t attributeByte = (back << 4) | (fore & 0x0F);
	uint16_t attribute = attributeByte << 8;

	if (c == 0x08 && cursor_x) // ascii backspace = 0x08
	{
		vram[cursor_y * 80 + cursor_x - 1] = ' ' | attribute;
		cursor_x--;
	}
	else if (c == 0x09) // ascii tab = 0x09
	{
		cursor_x = (cursor_x + 8) & ~(8 - 1); //和~(0111)的反，即（1000）按位与，相当于[(cursor_x+8)%8]-1
	}
	else if (c == '\r') //回车，并不会换行，只是回到句首
	{
		cursor_x = 0;
	}
	else if (c >= ' ')
	{
		vram[cursor_y * 80 + cursor_x] = c | attribute;
		cursor_x++;
	}
	else if (c == '\n') // 换行并回到句首
	{
		cursor_x = 0;
		cursor_y = cursor_y + 1;
	}

	if (cursor_x >= 80)
	{
		cursor_x = 0;
		cursor_y = cursor_y + 1;
	}

	scroll();
	move_cursor();

	console_release_lock();
}

// print a null-terminated string , back=black , fore=white
void console_write(char *cstr)
{
	// console_acquire_lock();
	while (*cstr)
	{
		console_put_color(*cstr++, black, white);
	}
	// console_release_lock();
}

// print a null-terminated string with color
void console_write_color(char *cstr, color back_color, color fore_color)
{
	// console_acquire_lock();
	while (*cstr)
	{
		console_put_color(*cstr++, back_color, fore_color);
	}
	// console_release_lock();
}

// print a hex num
void console_write_hex(uint32_t num, color back_color, color fore_color)
{
	char cstr[10]; //二进制32位转换为十六进制最大值为8位数（0xffffffff)

	int index;
	index = 0;

	cstr[index] = '0';
	index++;
	cstr[index] = 'x';
	index++;

	char alphabet[16];

	alphabet[0] = '0';
	alphabet[1] = '1';
	alphabet[2] = '2';
	alphabet[3] = '3';
	alphabet[4] = '4';
	alphabet[5] = '5';
	alphabet[6] = '6';
	alphabet[7] = '7';
	alphabet[8] = '8';
	alphabet[9] = '9';
	alphabet[10] = 'a';
	alphabet[11] = 'b';
	alphabet[12] = 'c';
	alphabet[13] = 'd';
	alphabet[14] = 'e';
	alphabet[15] = 'f';

	int k;

	for (k = 0; k < 8; k++)
	{
		cstr[9 - k] = alphabet[num % 16];
		num = num / 16;
	}

	console_write_color(cstr, back_color, fore_color);
}

// print a decimal num
void console_write_dec(uint32_t num, color back_color, color fore_color)
{
	char cstr[10]; //二进制32位转换为十进制最大值为10位数

	uint32_t digit;
	digit = 1000000000;

	uint32_t index;
	index = 0;

	char alphabet[10];

	alphabet[0] = '0';
	alphabet[1] = '1';
	alphabet[2] = '2';
	alphabet[3] = '3';
	alphabet[4] = '4';
	alphabet[5] = '5';
	alphabet[6] = '6';
	alphabet[7] = '7';
	alphabet[8] = '8';
	alphabet[9] = '9';

	// while (TRUE)
	// {
	// 	if ((num / digit) != 0)
	// 	{
	// 		break;
	// 	}
	// 	digit = digit / 10;
	// }

	// do
	// {
	// 	cstr[index] = alphabet[num / digit];
	// 	index++;
	// 	num = num % digit;
	// 	digit = digit / 10;

	// 	if (index >= 10)
	// 		break;

	// } while (digit != 0);
	char cstr_real[10];

	for (int i = 0; i < 10; i++)
	{
		cstr_real[i] = 0;
	}

	int k;
	int flag = 0;

	for (k = 0; k <= 9; k++)
	{
		cstr[9 - k] = alphabet[num % 10];
		num = num / 10;
	}

	for (int i = 0; i < 10; i++)
	{
		if (cstr[i] != '0')
		{
			int j;
			for (j = 0; j < 10 - i; j++)
			{
				cstr_real[j] = cstr[i + j];
			}

			flag = 1;

			break;
		}
	}

	if (flag == 0)
	{
		cstr_real[0] = '0';
	}

	console_write_color(cstr_real, back_color, fore_color);
}

extern void panic(const char *message, const char *file, uint32_t line)
{
	// We encountered a massive problem and have to stop.
	asm volatile("cli"); // Disable interrupts.

	console_write_color("PANIC", cyan, red);
	console_put_color('(', black, white);
	console_write(message);
	console_write(") at ");
	console_write(file);
	console_write(":");
	console_write_dec(line, black, white);
	console_write("\n");
	// going into an infinite loop.
	for (;;)
		;
}

void console_init()
{
	lock_init(&console_lock);
	// console_acquire_lock();
	// console_release_lock();
}

void console_acquire_lock()
{
	lock_acquire(&console_lock);
}

void console_release_lock()
{
	lock_release(&console_lock);
}
