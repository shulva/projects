#include "../header/types.h"
#include "../header/console.h"
#include "../header/gdt.h"
#include "../header/idt.h"
#include "../header/physical_memory.h"
#include "../header/timer.h"
#include "../header/multiboot.h"
#include "../header/heap.h"
#include "../header/thread.h"
#include "../header/keyboard.h"
#include "../header/filesystem.h"
#include "../header/shell.h"

void printa(void *arg)
{
	while (1)
	{
		interrupt_status old_status = interrupt_disable();

		if (!ring_buffer_is_empty(&keyboard_buffer))
		{
			console_write(arg);
			char byte = getchar_from_buffer(&keyboard_buffer);
			console_put_color(byte, black, white);
		}

		interrupt_set_status(old_status);
	}
}
void printb(void *arg)
{
	while (1)
	{
		interrupt_status old_status = interrupt_disable();

		if (!ring_buffer_is_empty(&keyboard_buffer))
		{
			console_write(arg);
			char byte = getchar_from_buffer(&keyboard_buffer);
			console_put_color(byte, black, white);
		}

		interrupt_set_status(old_status);
	}
}

int kern_entry() // struct multiboot *glb_mboot_ptr
{
	console_init();

	console_clear();

	keyboard_init();

	init_gdt();
	info;
	console_write_color("Global Descriptor Table Initialize\n", black, white);
	init_idt();
	info;
	console_write_color("Interrupt Descriptor Table Initialize\n", black, white);

	loop_sleep(25);
	console_clear();

	init_memory();
	thread_init();

	loop_sleep(25);
	console_clear();

	disk_init();

	loop_sleep(25);
	console_clear();

	filesystem_init();

	loop_sleep(30);
	console_clear();

	init_timer(100); // 约为+1s-100tciks

	interrupt_enable();

	console_write("\n");
	info;
	console_write_color("kernel in memory start:", black, white);
	console_write_hex((uint32_t)kernel_start, black, white);

	console_write("\n");
	info;
	console_write_color("kernel in memory end:", black, white);
	console_write_hex((uint32_t)kernel_end, black, white);

	console_write("\n");
	info;
	console_write_color("kernel in memory placed:", black, white);
	console_write_dec((kernel_end - kernel_start + 1024) / 1024, black, white);
	console_write("kb\n");

	show_memory_map();

	loop_sleep(30);
	console_clear();

	// for (int i = 0; i < 80; i++)
	// {
	// 	console_put_color('l', black, white);
	// }

	// heap test
	// uint32_t a = kmalloc(8);
	// interrupt_disable();

	// console_write_hex(get_running_thkread_pcb(),black,white);

	// thread_start("threadA", 2, printa, "A-");
	// thread_start("threadB", 2, printb, "B-");

	interrupt_enable();
	print_prompt();

	// while (1)
	// {
	// 	console_write("M ");
	// }

	// uint32_t b = kmalloc(10);

	// uint32_t c = kmalloc(8);

	// console_write("\na: ");
	// console_write_hex(a, black, white);
	// console_write(", b: ");
	// console_write_hex(b, black, white);
	// console_write("\nc: ");
	// console_write_hex(c, black, white);

	// kfree(c);
	// kfree(b);

	// uint32_t d = kmalloc(12);
	// console_write("\d: ");
	// console_write_hex(d, black, white);

	// asm volatile("int $0x03");

	// uint32_t *ptr = (uint32_t *)0xA0000000;
	// uint32_t do_page_fault = *ptr;

	return 0;
}
