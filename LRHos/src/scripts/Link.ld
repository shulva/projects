/* linker script
 * '.' 表示当前地址，可赋值，也可被赋值
 * *(.(data|bss|text|rodata)) 表示会将所有的目标的段链接到输出文件的对应的段中
 *  align(n) 起到对齐的作用，不足的地方会自动补充byte
 *  entry tells ld that the start location of our binary should be the symbol 'start' (入口地址)
 *  .text start at 0x100000(1mb)
 *  each should be page-aligned (ALIGN(4096)) 
 *  gcc also adds in an extra data section: .rodata
 *  .rodata os for readonly initialised data such as constant 
 *
 */  
 

ENTRY(start)

SECTIONS
{
	. = 0x100000;
	PROVIDE( kernel_start = . );
	
	.text :
	{
		code = .; _code = .; __code = .;
		*(.text)
		.  = ALIGN(4096);
	}

	.data :
	{
		data = .; _data = .; __data = .;
		*(.data)
		*(.rodata)
		. = ALIGN(4096);
	}

	.bss :
	{
		bss = .; _bss = .; __bss = .;
		*(.bss)
		. = ALIGN(4096);
	}

	.stab :
    {
        *(.stab)
        . = ALIGN(4096);
    }

    .stabstr :
    {
        *(.stabstr)
        . = ALIGN(4096);
    }

	PROVIDE( kernel_end = . );

	/DISCARD/ : { *(.comment) *(.eh_frame) }

}
