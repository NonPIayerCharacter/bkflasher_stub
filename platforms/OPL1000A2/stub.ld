ENTRY(flasher_stub)

MEMORY
{
	RAM (rwx) : ORIGIN = 0x00440400, LENGTH = 0x0000F000
	DRAM (rwx) : ORIGIN = 0x004164A0, LENGTH = 0x00440400 - 0x004164A0
}

SECTIONS
{
	.init ORIGIN(RAM) : ALIGN(4)
	{
		KEEP(*(.init))
	} > RAM

	.text : ALIGN(4)
	{
		*(.text*)
		*(.rodata*)
	} > RAM

	.data : ALIGN(4)
	{
		*(.data*)
	} > RAM

	.bss (NOLOAD) : ALIGN(4)
	{
		__bss_start__ = .;
		*(.bss*)
		*(COMMON)
		__bss_end__ = .;
	} > RAM

	.dram (NOLOAD) : ALIGN(4)
	{
		*(.dram*)
	} > DRAM

	/DISCARD/ :
	{
		*(.ARM.exidx*)
		*(.ARM.extab*)
		*(.comment*)
		*(.note*)
	}
}

SECTIONS
{
	memset                               = 0x00000379;
	memcpy                               = 0x00075247;
	tc_sha256_final                      = 0x0008e2a5;
	tc_sha256_init                       = 0x0008e375;
	tc_sha256_update                     = 0x0008e3cd;
	Crc32Table                           = 0x000b521c;
	Hal_Sys_RetRamTurnOn                 = 0x00400708;
	Hal_Sys_ApsClkTreeSetup              = 0x00400720;
	Hal_Sys_OtpRead                      = 0x00400784;
	_Hal_Flash_ManufDeviceId             = 0x004007fc;
	Hal_Flash_4KSectorAddrErase_Internal = 0x00400814;
	Hal_Flash_AddrProgram_Internal       = 0x00400828;
	Hal_Flash_AddrRead_Internal          = 0x0040082c;
	Hal_DbgUart_BaudRateSet              = 0x0040096c;
}
