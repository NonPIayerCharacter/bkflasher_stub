#include "../hal_generic.h"

void uart_putc(uint8_t b)
{
	while(!UART_Writable(UART2_DEV));
	UART_CharPut(UART2_DEV, b);
}

int8_t uart_getc_timeout(uint8_t* out, uint32_t loops)
{
	while(loops--)
	{
		if(UART_Readable(UART2_DEV))
		{
			UART_CharGet(UART2_DEV, out);
			return 1;
		}
	}
	return 0;
}

void uart_set_baud(uint32_t baud)
{
	UART_SetBaud(UART2_DEV, baud);
}

void uart_init(void)
{
	uart_set_baud(115200U);
}

void flash_init(void)
{
	FLASH_CalibrationNewCmd(0);
	FLASH_ClockDiv(0);
	RCC_PeriphClockCmd(APBPeriph_FLASH, APBPeriph_FLASH_CLOCK, 1);
	PINMUX_Ctrl(10, 0, 1);
	FLASH_StructInit(&flash_init_para);
	if(!FLASH_Init(2)) if(!FLASH_Init(1)) FLASH_Init(0);

	uint8_t flash_ID[3];
	FLASH_RxCmd(flash_init_para.FLASH_cmd_rd_id, 3, flash_ID);
	FLASH_RxCmd(flash_init_para.FLASH_cmd_rd_id, 3, (uint8_t*)&flash_id);
	if(flash_ID[0] == 0x20)
	{
		flash_init_para.FLASH_cmd_chip_e = 0xC7;
	}

	Cache_Enable(1);
}

int FLASH_WriteStream(uint32_t address, uint32_t len, const uint8_t* data)
{
	// Check address: 4byte aligned & page(256bytes) aligned
	uint32_t page_begin = address & (~0xff);
	uint32_t page_end = (address + len) & (~0xff);
	uint32_t page_cnt = ((page_end - page_begin) >> 8) + 1;

	uint32_t addr_begin = address;
	uint32_t addr_end = (page_cnt == 1) ? (address + len) : (page_begin + 0x100);
	uint32_t size = addr_end - addr_begin;
	const uint8_t* buffer = data;
	uint8_t write_data[12];

	uint32_t offset_to_align;
	uint32_t read_word;
	uint32_t i;

	while(page_cnt)
	{
		offset_to_align = addr_begin & 0x3;

		if(offset_to_align != 0)
		{
			read_word = REG32(FLASH_BASE + addr_begin - offset_to_align);
			for(i = offset_to_align; i < 4; i++)
			{
				read_word = (read_word & (~(0xff << (8 * i)))) | ((*buffer) << (8 * i));
				size--;
				buffer++;
				if(size == 0)
					break;
			}
			FLASH_TxData12B(addr_begin - offset_to_align, 4, (uint8_t*)&read_word);
		}

		addr_begin = (((addr_begin - 1) >> 2) + 1) << 2;
		for(; size >= 12; size -= 12)
		{
			memcpy(write_data, buffer, 12);
			FLASH_TxData12B(addr_begin, 12, write_data);

			buffer += 12;
			addr_begin += 12;
		}

		for(; size >= 4; size -= 4)
		{
			memcpy(write_data, buffer, 4);
			FLASH_TxData12B(addr_begin, 4, write_data);

			buffer += 4;
			addr_begin += 4;
		}

		if(size > 0)
		{
			read_word = REG32(FLASH_BASE + addr_begin);
			for(i = 0; i < size; i++)
			{
				read_word = (read_word & (~(0xff << (8 * i)))) | ((*buffer) << (8 * i));
				buffer++;
			}
			FLASH_TxData12B(addr_begin, 4, (uint8_t*)&read_word);
		}

		page_cnt--;
		addr_begin = addr_end;
		addr_end = (page_cnt == 1) ? (address + len) : (((addr_begin >> 8) + 1) << 8);
		size = addr_end - addr_begin;
	}

	Cache_Flush();

	return 1;
}

void flash_program_page(uint32_t off, const uint8_t* data)
{
	FLASH_WriteStream(off, FLASH_PAGE_SIZE, data);
	Cache_Flush();
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
		if((cur % 0x10000U == 0) && (end - cur >= 0x10000U))
		{
			FLASH_Erase(EraseBlock, cur);
			cur += 0x10000U;
		}
		else
		{
			FLASH_Erase(EraseSector, cur);
			cur += 0x1000U;
		}
	}

	return 1;
}

int flash_erase_chip()
{
	FLASH_Erase(EraseChip, 0);
	return 1;
}

int crc32_memory_hardware(uint32_t addr, uint32_t len, uint32_t* result)
{
	*result = crc32_get((void*)addr, len);
	return 1;
}

int sha256_memory_hardware(uint32_t addr, uint32_t len)
{
	__attribute__((aligned(8))) uint8_t hash[32] = { 0 };
	sha256((void*)addr, len, hash, 0);
	memcpy(cmd_buf, hash, 32);
	return 1;
}

int read_efuse(void)
{
	EFUSE_LogicalMap_Read((uint8_t*)&cmd_buf);
	return 512;
}

int read_factory_mac(uint8_t mac[6])
{
	return 0;
}

extern int main(void);

__attribute__((used)) void flasher_stub(void)
{
	VECTOR_IrqDis(28);
	memset((void*)__image1_bss_start__, 0, (__image1_bss_end__ - __image1_bss_start__));
	RCC_PeriphClockCmd(APBPeriph_FLASH, APBPeriph_FLASH_CLOCK, 0);
	CPU_ClkSet(0);
	DelayUs(10);
	Cache_Enable(0);
	main();
}

__attribute__((section(".image1.entry.data"), used))
RAM_FUNCTION_START_TABLE RamStartTable = {
	.RamStartFun = flasher_stub,
	.RamWakeupFun = flasher_stub,
	.RamPatchFun0 = flasher_stub,
	.RamPatchFun1 = flasher_stub,
	.RamPatchFun2 = flasher_stub
};
