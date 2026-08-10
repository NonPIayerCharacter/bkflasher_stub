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

void flash_rx_mode_switch(uint8_t read_mode)
{
	uint8_t tmp_dc = 0, status = 0, spic_mode = 0, i;
	uint32_t pdata[2];
	pdata[0] = 0x96969999;
	pdata[1] = 0xFC66CC3F;

	for(i = read_mode; i < 5; i++)
	{
		if(i == ReadQuadIOMode)
		{
			flash_init_para.FALSH_quad_valid_cmd = (BIT_WR_BLOCKING | BIT_RD_QUAD_IO);

			tmp_dc = flash_init_para.FLASH_rd_dummy_cyle[2];
			flash_init_para.FLASH_rd_dummy_cyle[2] -= QUAD_PRM_CYCLE_NUM;

			spic_mode = SpicQuadBitMode;
			flash_init_para.FLASH_cur_cmd = FLASH_CMD_4READ;
		}
		else if(i == ReadQuadOMode)
		{
			flash_init_para.FALSH_quad_valid_cmd = (BIT_WR_BLOCKING | BIT_RD_QUAD_O);

			flash_init_para.FLASH_rd_dummy_cyle[2] = FLASH_DM_CYCLE_4O;
			tmp_dc = flash_init_para.FLASH_rd_dummy_cyle[2];

			spic_mode = SpicQuadBitMode;
			flash_init_para.FLASH_cur_cmd = FLASH_CMD_QREAD;
		}
		else if(i == ReadDualIOMode)
		{
			flash_init_para.FALSH_dual_valid_cmd = (BIT_WR_BLOCKING | BIT_FRD_SINGEL);

			tmp_dc = flash_init_para.FLASH_rd_dummy_cyle[1];
			flash_init_para.FLASH_rd_dummy_cyle[1] -= DUAL_PRM_CYCLE_NUM;

			spic_mode = SpicDualBitMode;
			flash_init_para.FLASH_cur_cmd = FLASH_CMD_2READ;
		}
		else if(i == ReadDualOMode)
		{
			flash_init_para.FALSH_dual_valid_cmd = (BIT_WR_BLOCKING | BIT_RD_DUAL_I);

			flash_init_para.FLASH_rd_dummy_cyle[1] = FLASH_DM_CYCLE_2O;
			tmp_dc = flash_init_para.FLASH_rd_dummy_cyle[1];

			spic_mode = SpicDualBitMode;
			flash_init_para.FLASH_cur_cmd = FLASH_CMD_DREAD;
		}
		else
		{
			tmp_dc = flash_init_para.FLASH_rd_dummy_cyle[0];
			spic_mode = SpicOneBitMode;
			flash_init_para.FLASH_cur_cmd = FLASH_CMD_READ;
		}

		if(flash_init_para.FLASH_Id == FLASH_ID_MICRON)
		{
			FLASH_RxCmd(0x85, 1, &status);

			status = (status & 0x0f) | (tmp_dc << 4);
			FLASH_SetStatus(0x81, 1, &status);
		}

		flash_init_para.FLASH_rd_sample_phase = SPIC_LOWSPEED_SAMPLE_PHASE;
		Cache_Enable(0);
		RCC_PeriphClockCmd(APBPeriph_FLASH, APBPeriph_FLASH_CLOCK, 0);
		FLASH_CalibrationNewCmd(0);
		FLASH_ClockDiv(0);
		RCC_PeriphClockCmd(APBPeriph_FLASH, APBPeriph_FLASH_CLOCK, 1);
		FLASH_Init(spic_mode);
		Cache_Enable(1);

		Cache_Flush();

		if(memcmp((void*)FLASH_BASE, pdata, 8) == 0)
		{
			break;
		}
	}
}

void flash_init(void)
{
	Cache_Enable(0);
	PINMUX_Ctrl(10, 0, 1);
	FLASH_StructInit(&flash_init_para);
	flash_rx_mode_switch(0);
	FLASH_RxCmd(flash_init_para.FLASH_cmd_rd_id, 3, (uint8_t*)&flash_id);
	if((flash_id & 0xFF) == 0x20)
	{
		flash_init_para.FLASH_cmd_chip_e = 0xC7;
	}
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

void get_chip_data(void)
{
	WRITE_REG32(cmd_buf, 0x43B186D6); // RTL8710B
	// todo: get chip id
}

extern int main(void);

__attribute__((used)) void flasher_stub(void)
{
	__asm volatile ("cpsid i" : : : "memory");
	memset((void*)__image1_bss_start__, 0, (__image1_bss_end__ - __image1_bss_start__));
	RCC_PeriphClockCmd(APBPeriph_FLASH, APBPeriph_FLASH_CLOCK, 0);
	CPU_ClkSet(0);
	DelayUs(10);
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
