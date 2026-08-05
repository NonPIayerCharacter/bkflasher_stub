MEMORY
{
	ilm_memory    (rwx) : org = 0x00010000, len = 192K
	dlm_memory    (!rx) : org = 0x00060000, len = 128K
	iram_memory   (rwx) : org = 0x00080000, len = 192K
}

ENTRY(_boot_start);

SECTIONS
{
    .nds32_init ORIGIN(ilm_memory) :
    {
        _nds32_init_start = ABSOLUTE(.);
        KEEP (*(.nds32_init))
        _nds32_init_end = ABSOLUTE(.);
    } > ilm_memory 
  
    .text : ALIGN(4)
    {
        *(.text*)
        *(.rodata*)
    } > ilm_memory

    .data : ALIGN(4)
    {
        *(.data*)
    } > ilm_memory

    .bss (NOLOAD) : ALIGN(4)
    {
        __bss_start__ = .;
        *(.bss*)
        __bss_end__ = .;
    } > ilm_memory

    .dlm (NOLOAD) :
    {
        *(.dlm*)
        *(.dram2*)
    } > dlm_memory

    .iram (NOLOAD) :
    {
        *(.iram*)
        *(.dram1*)
    } > iram_memory

    PROVIDE (_stack = 0x0007F9F0);
}

SECTIONS
{
    memcpy = 0x00000250;
    memset = 0x00000400;
    sha256 = 0x000087fa;
    udelay = 0x000088bc;
    spi_bus_ready = 0x000088ea;
    spi_fifo_reset = 0x00008906;
    spi_rx_ready = 0x0000892e;
    uart_getc = 0x00008a2e;
    uart_data_tstc = 0x00008b32;
    //uart_init_ecr = 0x00008baa;
    efuse_read = 0x00008eb6;
    efuse_read_series = 0x00008efa;
    efuse_cfg_160m = 0x00008f5a;
    crc32 = 0x00009020;
}
