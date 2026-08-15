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
	XTAL_ClkGet = 0x20281;
	PLL_ClkGet = 0x202c9;
	CPU_ClkGet = 0x2031d;
	CRYPTO_SHA_Init = 0x21011;
	DelayUs = 0x21c1d;
	FLASH_SetSpiMode = 0x22579;
	FLASH_RxCmd = 0x22691;
	FLASH_RxData = 0x227bd;
	FLASH_TxCmd = 0x22891;
	FLASH_TxData = 0x2291d;
	FLASH_Erase = 0x22a39;
	FLASH_DeepPowerDown = 0x22af1;
	FLASH_StructInit = 0x22e31;
	LOGUART_SetBaud = 0x23a45;
	RCC_PeriphClockCmd = 0x24d01;
	RCC_PeriphClockSource_SPIC = 0x24e1d;
	RCC_PeriphClockSource_PSRAM = 0x24e39;
	rtl_crypto_sha2_init = 0x25295;
	rtl_crypto_sha2_process = 0x254c1;
	rtl_crypto_sha2_update_rom = 0x254fd;
	rtl_crypto_sha2_update = 0x25571;
	rtl_crypto_sha2_final = 0x255d5;
	SYSCFG_OTP_SPICAddr4ByteEn = 0x25edd;
	DiagPrintf = 0x283cd;
	DiagSnPrintf = 0x28469;
	memcpy = 0x28f1d;
	memset = 0x290e5;
	RBSS_UDELAY_CLK = 0x20001038;
	flash_init_para = 0x20004014;
}
