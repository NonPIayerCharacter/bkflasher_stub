MEMORY
{
	IRAM (rwx) : ORIGIN = 0x1FFE0000, LENGTH = 0x48000 - 0x100
}

ENTRY(Reset_Handler)

SECTIONS
{
	.text ORIGIN(IRAM) : ALIGN(4)
	{
	KEEP(*(.boot_header))
	. = ALIGN(0x100);
		KEEP(*(.vectors))
		KEEP(*(.Reset_Handler))
		*(.text*)
		*(.rodata*)
	} > IRAM

	.data : ALIGN(4)
	{
		*(.data*)
	} > IRAM

	.bss (NOLOAD) : ALIGN(4)
	{
		__bss_start__ = .;
		*(.bss*)
		__bss_end__ = .;
	} > IRAM
	__StackTop = ORIGIN(IRAM) + LENGTH(IRAM);
}
