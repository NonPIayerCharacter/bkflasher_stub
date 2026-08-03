ENTRY(Reset_Handler)

MEMORY
{
	BOOTLOADER_RAM (rwx) : ORIGIN = 0x000082000, LENGTH = 0xD000
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
	Reset_Handler           = 0x101;
	DelayUs                 = 0x14c3;
	EFUSE_LogicalMap_Read   = 0x1965;
	FLASH_SW_CS_Control     = 0x1c51;
	FLASH_RxCmd             = 0x1d1d;
	FLASH_WaitBusy          = 0x1d89;
	FLASH_WriteEn           = 0x1f3d;
	FLASH_TxData256B        = 0x1f61;
	FLASH_TxData12B         = 0x2049;
	FLASH_SetStatus         = 0x2129;
	FLASH_Erase             = 0x2147;
	FLASH_StructInit        = 0x2451;
	FLASH_Init              = 0x24e1;
	UART_StructInit         = 0x403f;
	UART_SetBaud            = 0x40b1;
	UART_Writable           = 0x41ed;
	UART_Readable           = 0x41f5;
	UART_CharPut            = 0x41fd;
	UART_CharGet            = 0x4201;
	UART_Init               = 0x4307;
	CPU_ClkSet              = 0x4a11;
	memcmp                  = 0x6545;
	memcpy                  = 0x6591;
	memset                  = 0x66ad;
	SPIC_CALIB_PATTERN      = 0x20064;
	flash_init_para         = 0x801f8;
}
