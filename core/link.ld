OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS
{
	virtual_base = 0xffffffff80000000;
	. = virtual_base + 2M;

	kernel_start = .;
	.stivale2hdr : ALIGN(0x1000)
	{
        KEEP(*(.stivale2hdr))
	}
	.text : ALIGN(0x1000)
	{
		*(.text)
	}
	.data : ALIGN(0x1000)
	{
		*(.data)
	}
	.rodata : ALIGN(0x1000)
	{
		*(.rodata)
	}
	.bss : ALIGN(0x1000)
	{
		*(COMMON)
		*(.bss)
	}
	kernel_end = .;
}