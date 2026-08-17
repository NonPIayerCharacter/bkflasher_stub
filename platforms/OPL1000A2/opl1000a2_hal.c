#include "../hal_generic.h"

void uart_putc(uint8_t b)
{
	while(DBG_UART->STATE & DBG_UART_STATE_TX_FULL);
	DBG_UART->DATA = (char)b;
	DBG_UART->INT_STATUS |= DBG_UART_INT_STATUS_TX;
}

int8_t uart_getc_timeout(uint8_t* out, uint32_t loops)
{
	while(loops--)
	{
		if((DBG_UART->STATE & DBG_UART_STATE_RX_FULL) == DBG_UART_STATE_RX_FULL)
		{
			*out = DBG_UART->DATA;
			return 1;
		}
	}
	return 0;
}

void uart_set_baud(uint32_t baud)
{
	Hal_DbgUart_BaudRateSet(baud);
}

void flash_init(void)
{
	uint32_t flashId[3];
	_Hal_Flash_ManufDeviceId(0, &flashId[0], &flashId[1], &flashId[2]);
	flash_id = ((flashId[2] & 0xFF) << 16) | ((flashId[1] & 0xFF) << 8) | (flashId[0] & 0xFF);
}

void flash_program_page(uint32_t off, const uint8_t* data)
{
	Hal_Flash_AddrProgram_Internal(0, off, 0, FLASH_PAGE_SIZE, (uint8_t*)data);
}

int flash_erase_range(uint32_t src, uint32_t len)
{
	if((src & 0x3) != 0)
		return 0;
	if(len == 0)
		return 0;

	uint32_t cur = src;
	uint32_t end = src + len;

	while(cur < end)
	{
		Hal_Flash_4KSectorAddrErase_Internal(0, cur);
		cur += 0x1000U;
	}

	return 1;
}

int flash_erase_chip()
{
	flash_erase_range(0, flash_size);
	return 1;
}

void stub_flash_read(void* dest, uint32_t off, size_t len)
{
	if(off < FLASH_BASE) memcpy(dest, (const void*)(off), len);
	else Hal_Flash_AddrRead_Internal(0, off - FLASH_BASE, 0, len, dest);
}

int crc32_memory(uint32_t addr, uint32_t len, uint32_t* result)
{
	uint32_t crc = 0xFFFFFFFF;
	while(len > 0)
	{
		uint32_t chunk = len > BUF_SIZE ? BUF_SIZE : len;
		uint32_t remaining = chunk;
		stub_flash_read(cmd_buf, addr, chunk);
		const uint8_t* p = cmd_buf;
		while(remaining >= 4)
		{
			crc = (crc >> 8) ^ Crc32Table[(crc ^ *p++) & 0xFF];
			crc = (crc >> 8) ^ Crc32Table[(crc ^ *p++) & 0xFF];
			crc = (crc >> 8) ^ Crc32Table[(crc ^ *p++) & 0xFF];
			crc = (crc >> 8) ^ Crc32Table[(crc ^ *p++) & 0xFF];
			remaining -= 4;
		}
		while(remaining--)
			crc = (crc >> 8) ^ Crc32Table[(crc ^ *p++) & 0xFF];
		addr += chunk;
		len -= chunk;
	}
	*result = crc ^ 0xffffffff;
	return 1;
}

int sha256_memory_hardware(uint32_t addr, uint32_t len)
{
	uint8_t hash[32] = { 0 };
	struct tc_sha256_state_struct ctx = { 0 };
	tc_sha256_init(&ctx);
	uint32_t remaining = len;
	while(remaining > 0)
	{
		uint32_t chunk = remaining > BUF_SIZE ? BUF_SIZE : remaining;
		stub_flash_read(cmd_buf, addr, chunk);
		tc_sha256_update(&ctx, cmd_buf, chunk);
		addr += chunk;
		remaining -= chunk;
	}
	tc_sha256_final(hash, &ctx);
	memcpy(cmd_buf, hash, 32);
	return 1;
}

void get_chip_data(void)
{
	WRITE_REG32(cmd_buf, 0xAA5D6AC8); // OPL1000A2
}

int read_efuse(void)
{
	Hal_Sys_OtpRead(0, cmd_buf, 0x200);
	return 0x200;
}

extern int main(void);

__attribute__((section(".init"), used))
void flasher_stub(void)
{
	__asm volatile ("cpsid i" : : : "memory");
	Hal_Sys_RetRamTurnOn(0);
	Hal_Sys_ApsClkTreeSetup(APS_CLKTREE_SRC_XTAL_X2, 0, 0);
	//Hal_Sys_ApsClkTreeSetup(APS_CLKTREE_SRC_1P2G_MAX, 1, 0);
	memset((void*)__bss_start__, 0, (__bss_end__ - __bss_start__));
	main();
}
