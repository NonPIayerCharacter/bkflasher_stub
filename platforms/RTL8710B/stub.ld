ENTRY(Reset_Handler)

MEMORY
{
	BOOTLOADER_RAM (rwx) : ORIGIN = 0x10002000, LENGTH = 0x3C000
}

SECTIONS
{
	/* image1 entry, this section should in RAM and fixed address for ROM */
	.ram_image1.entry :
	{
		__ram_image1_text_start__ = .;
		__ram_start_table_start__ = .;
		KEEP(*(SORT(.image1.entry.data*)))
		__ram_start_table_end__ = .;

		__image1_validate_code__ = .;
		KEEP(*(.image1.validate.rodata*))
		KEEP(*(.image1.export.symb*))
	} > BOOTLOADER_RAM

	.text . :
	{
		*(.text*)
		*(.rodata*)
	} > BOOTLOADER_RAM

	.data . :
	{
		__ram_image1_data_start__ = .;
		KEEP(*(.data*))
		__ram_image1_data_end__ = .;
	
		__ram_image1_text_end__ = .;
	} > BOOTLOADER_RAM

	.bss . :
	{
		__image1_bss_start__ = .;
		KEEP(*(.bss*))
		__image1_bss_end__ = .;
	} > BOOTLOADER_RAM
}

SECTIONS
{
	Reset_Handler = 0x101;
	memcmp = 0x991;
	memcpy = 0x9c5;
	memset = 0xa7d;
	CPU_ClkSet = 0x24b1;
	DelayUs = 0x3411;
	PINMUX_Ctrl = 0x42fd;
	Cache_Enable = 0x5811;
	Cache_Flush = 0x5831;
	Cache_Debug = 0x5851;
	EFUSE_LogicalMap_Read = 0x7091;
	FLASH_RxCmd = 0x7465;
	FLASH_WaitBusy = 0x74cd;
	FLASH_TxData12B = 0x7661;
	FLASH_SetStatus = 0x7735;
	FLASH_Erase = 0x7755;
	FLASH_StructInit = 0x7c11;
	FLASH_Init = 0x7ca1;
	FLASH_ClockDiv = 0x7d15;
	FLASH_CalibrationNewCmd = 0x7e6d;
	RCC_PeriphClockCmd = 0x9b65;
	UART_StructInit = 0xbe69;
	UART_SetBaud = 0xbf01;
	UART_Writable = 0xbffd;
	UART_Readable = 0xc005;
	UART_CharPut = 0xc00d;
	UART_CharGet = 0xc011;
	UART_Init = 0xc135;
	crc32_get = 0x11549;
	sha256 = 0x4524d;
	flash_init_para = 0x10000dec;
	boot_export_symbol = 0x10002020;  
}
