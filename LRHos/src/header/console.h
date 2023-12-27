#ifndef HEADER_CONSOLE_H
#define HEADER_CONSOLE_H

#include "types.h"
#include "port.h"
#include "sync.h"

// the marco of assert is here
#define assert(x, info)          \
	do                           \
	{                            \
		if (!(x))                \
		{                        \
			console_write(info); \
		}                        \
	} while (0)

#define PANIC(msg) panic(msg, __FILE__, __LINE__);

#define info console_write_color("[info]:",black,light_green);

static struct lock console_lock;

enum color
{
	black = 0,
	blue = 1,
	green = 2,
	cyan = 3, //青色
	red = 4,
	magenta = 5, //洋红
	brown = 6,
	light_grey = 7,
	dark_grey = 8,
	light_blue = 9,
	light_green = 10,
	light_cyan = 11,
	light_red = 12,
	light_magenta = 13,
	light_brown = 14,
	white = 15
};

typedef enum color color;

// clear
void console_clear();

// print a null-terminated string , back=black , fore=white
void console_write(char *cstr);

// print a null-terminated string with color
void console_write_color(char *cstr, color back_color, color fore_color);

// print a hex num
void console_write_hex(uint32_t num, color back_color, color fore_color);

// print a decimal num
void console_write_dec(uint32_t num, color back_color, color fore_color);

// print a character with color
void console_put_color(char c, color back_color, color fore_color);

extern void panic(const char *message, const char *file, uint32_t line);

// operate lock...
void console_init();

void console_acquire_lock();

void console_release_lock();

#endif
