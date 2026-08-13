ENTRY(_boot_start);

MEMORY
{
	IRAM (rwx) : ORIGIN = 0x1C000, LENGTH = 0xC000
	DRAM (rwx) : ORIGIN = 0x200000, LENGTH = 0x24000
	DRAM1 (rwx) : ORIGIN = 0x240000, LENGTH = 0x10000
}

SECTIONS
{
	.nds32_init ORIGIN(IRAM) :
	{
		_nds32_init_start = ABSOLUTE(.);
		KEEP (*(.nds32_init))
		_nds32_init_end = ABSOLUTE(.);
	} > IRAM 
	
	.text : ALIGN(4)
	{
		*(.text.startup*)
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
		*(COMMON)
		. = ALIGN(4);
		__bss_end__ = .;
	} > IRAM

	.dram (NOLOAD) :
	{
		*(.dram2*)
	} > DRAM

	.dram1 (NOLOAD) :
	{
		*(.dram1*)
	} > DRAM1
	
	PROVIDE (_stack = 0x00223ff8);
}

SECTIONS
{
	memcpy = 0x1038;
	memset = 0x1114;
	uart_data_write = 0x3352;
	uart_data_tstc = 0x337e;
	uart_data_getc = 0x3386;
	spiFlash_init = 0x34b2;
	crc32 = 0x381e;
	spi_bus_ready = 0x338e;
	spi_fifo_reset = 0x33aa;
	spi_rx_ready = 0x33d4;
	_nds32_enable_dcache = 0x3258;
}
