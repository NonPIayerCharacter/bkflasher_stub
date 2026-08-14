ENTRY(Reset_Handler)

MEMORY
{
    BOOTLOADER_RAM (rwx) : ORIGIN = 0x3000A020, LENGTH = 0x3007FF00 - 0x3000A020
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

	__StackTop = ORIGIN(BOOTLOADER_RAM) + LENGTH(BOOTLOADER_RAM);
}

SECTIONS
{
	Reset_Handler = 0x101;
	EFUSE_GetChipVersion = 0x1f2c5;
	XTAL_ClkGet = 0x1f3e1;
	PLL_ClkGet = 0x1f3fd;
	CPU_ClkGet = 0x1f541;
	CRYPTO_SHA_Init = 0x20209;
	DelayUs = 0x20c05;
	DelayClkUpdate = 0x20c91;
	FLASH_SetSpiMode = 0x20d65;
	FLASH_RxCmd = 0x20ee9;
	FLASH_RxData = 0x20f8d;
	FLASH_TxData = 0x21131;
	FLASH_SetStatus = 0x2127d;
	FLASH_Erase = 0x212a9;
	FLASH_Addr4ByteEn = 0x21345;
	FLASH_DeepPowerDown = 0x2138d;
	FLASH_StructInit = 0x21611;
	FLASH_Init = 0x21681;
	LOGUART_SetBaud = 0x21c45;
	LOGUART_GetChar = 0x21eb1;
	LOGUART_GetStatus = 0x21f19;
	LOGUART_WaitTxComplete = 0x22135;
	OTP_LogicalMap_Read = 0x23185;
	PAD_PullCtrl = 0x23575;
	Pinmux_Config = 0x23769;
	RCC_PeriphClockCmd = 0x238d9;
	RCC_PeriphClockSource_SPIC = 0x23a01;
	rtl_crypto_sha2_init = 0x23ebd;
	rtl_crypto_sha2_process = 0x240ed;
	rtl_crypto_sha2_update_rom = 0x24129;
	rtl_crypto_sha2_update = 0x241a1;
	rtl_crypto_sha2_final = 0x24205;
	SYSCFG_ROMINFO_Get = 0x24a79;
	SYSCFG_CHIPType_Get = 0x24aa5;
	SYSCFG_OTP_SPICAddr4ByteEn = 0x24b8d;
	UART_StructInit = 0x251c1;
	UART_SetBaud = 0x25245;
	UART_RxCmd = 0x25395;
	UART_Writable = 0x253a9;
	UART_Readable = 0x253b1;
	UART_CharPut = 0x253b9;
	UART_CharGet = 0x253bd;
	UART_Init = 0x254d5;
	irq_table_init = 0x25c55;
	irq_enable = 0x25ecd;
	irq_disable = 0x25ee9;
	irq_register = 0x25ff5;
	irq_unregister = 0x26021;
	WDG_StructInit = 0x26079;
	WDG_Init = 0x26089;
	WDG_Refresh = 0x261c5;
	WDG_INTConfig = 0x2621d;
	WDG_ClearINT = 0x2628d;
	DiagPrintf = 0x265e1;
	DiagSnPrintf = 0x2667d;
	memcpy = 0x270e1;
	memset = 0x272a9;
	flash_init_para = 0x20004014;
}
