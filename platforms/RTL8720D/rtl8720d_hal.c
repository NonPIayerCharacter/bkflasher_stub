#include "../hal_generic.h"

uint32_t Crc32Table[256] = { 0 };

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

	for(i = read_mode; i < 5; i++)
	{
		if(i == ReadQuadIOMode)
		{
			flash_init_para.FLASH_pseudo_prm_en = 1;
			flash_init_para.FALSH_quad_valid_cmd = (BIT_WR_BLOCKING | BIT_RD_QUAD_IO | BIT_PRM_EN);

			tmp_dc = flash_init_para.FLASH_rd_dummy_cyle[2];
			flash_init_para.FLASH_rd_dummy_cyle[2] -= QUAD_PRM_CYCLE_NUM;

			spic_mode = SpicQuadBitMode;
			flash_init_para.FLASH_cur_cmd = FLASH_CMD_4READ;

		}
		else if(i == ReadQuadOMode)
		{
			flash_init_para.FLASH_pseudo_prm_en = 0;
			flash_init_para.FALSH_quad_valid_cmd = (BIT_WR_BLOCKING | BIT_RD_QUAD_O);
		
			flash_init_para.FLASH_rd_dummy_cyle[2] = FLASH_DM_CYCLE_4O;
			tmp_dc = flash_init_para.FLASH_rd_dummy_cyle[2];
		
			spic_mode = SpicQuadBitMode;
			flash_init_para.FLASH_cur_cmd = FLASH_CMD_QREAD;
		
		}
		else if(i == ReadDualIOMode)
		{
			flash_init_para.FLASH_pseudo_prm_en = 1;
			flash_init_para.FALSH_dual_valid_cmd = (BIT_WR_BLOCKING | BIT_FRD_SINGEL | BIT_CTRLR0_CH | BIT_PRM_EN);
		
			tmp_dc = flash_init_para.FLASH_rd_dummy_cyle[1];
			flash_init_para.FLASH_rd_dummy_cyle[1] -= DUAL_PRM_CYCLE_NUM;
		
			spic_mode = SpicDualBitMode;
			flash_init_para.FLASH_cur_cmd = FLASH_CMD_2READ;
		
		}
		else if(i == ReadDualOMode)
		{
			flash_init_para.FLASH_pseudo_prm_en = 0;
			flash_init_para.FALSH_dual_valid_cmd = (BIT_WR_BLOCKING | BIT_RD_DUAL_I);
		
			flash_init_para.FLASH_rd_dummy_cyle[1] = FLASH_DM_CYCLE_2O;
			tmp_dc = flash_init_para.FLASH_rd_dummy_cyle[1];
		
			spic_mode = SpicDualBitMode;
			flash_init_para.FLASH_cur_cmd = FLASH_CMD_DREAD;
		}
		else
		{
			flash_init_para.FLASH_pseudo_prm_en = 0;
		
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
		FLASH_Init(spic_mode);

		DCache_Invalidate(FLASH_BASE, 8);

		if(memcmp((void*)FLASH_BASE, SPIC_CALIB_PATTERN, 8) == 0)
		{
			break;
		}
	}
}

void flash_init(void)
{
	FLASH_StructInit(&flash_init_para);
	flash_rx_mode_switch(0);
	FLASH_RxCmd(flash_init_para.FLASH_cmd_rd_id, 3, (uint8_t*)&flash_id);
	if((flash_id & 0xFF) == 0x20)
	{
		flash_init_para.FLASH_cmd_chip_e = 0xC7;
	}
}

void FLASH_TxData256B_RAM(uint32_t StartAddr, uint32_t DataPhaseLen, uint8_t* pData)
{
	SPIC_TypeDef* spi_flash = SPIC;
	uint32_t tx_num = 0;
	uint32_t ctrl0 = spi_flash->ctrlr0;
	uint8_t addrbyte[4];
	uint8_t fifo_alarm_level = 28;

	/* write enable cmd */
	FLASH_WriteEn();

	/* disable SPI_FLASH user mode */
	spi_flash->ssienr = 0;

	/* set with TX mode and one bit mode */
	spi_flash->ctrlr0 &= ~(BIT_TMOD(3) | BIT_CMD_CH(3) | BIT_ADDR_CH(3) | BIT_DATA_CH(3));
	spi_flash->addr_length = flash_init_para.FLASH_addr_phase_len;

	/* set write cmd & address to fifo */
	addrbyte[3] = (StartAddr & 0xFF000000) >> 24;
	addrbyte[2] = (StartAddr & 0xFF0000) >> 16;
	addrbyte[1] = (StartAddr & 0xFF00) >> 8;
	addrbyte[0] = StartAddr & 0xFF;

	if(flash_init_para.FLASH_addr_phase_len == 3)
		spi_flash->dr[0].word = FLASH_CMD_PP | (addrbyte[2] << 8) | (addrbyte[1] << 16) | (addrbyte[0] << 24);
	else if(flash_init_para.FLASH_addr_phase_len == 0)
	{
		spi_flash->dr[0].byte = FLASH_CMD_PP;
		spi_flash->dr[0].word = (addrbyte[3]) | (addrbyte[2] << 8) | (addrbyte[1] << 16) | (addrbyte[0] << 24);
	}

	FLASH_SW_CS_Control(0);
	/* enable SPI_FLASH user mode */
	spi_flash->ssienr = 1;

	tx_num = 0;
	while(tx_num < DataPhaseLen)
	{
		/* flow control*/
		if((spi_flash->txflr & 0x1F) <= fifo_alarm_level)
		{
			spi_flash->dr[0].word = *(uint32_t*)(pData + tx_num);
			tx_num += 4;
		}
		else
		{
			while((spi_flash->sr & 0x3) == 0x3);
		}
	}
	FLASH_SW_CS_Control(1);

	/* wait spic busy done */
	FLASH_WaitBusy(0);

	/* disable SPI_FLASH user mode */
	spi_flash->ssienr = 0;

	/* wait write busy done */
	FLASH_WaitBusy(2);

	/* backup bitmode */
	spi_flash->ctrlr0 = ctrl0;
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
		//for(; size >= 256; size -= 256)
		//{
		//	FLASH_TxData256B_RAM(addr_begin, 256, data);
		//	data += 256;
		//	addr_begin += 256;
		//}
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

	DCache_Invalidate(FLASH_BASE + address, len);

	return 1;
}

void flash_program_page(uint32_t off, const uint8_t* data)
{
	FLASH_WriteStream(off, FLASH_PAGE_SIZE, data);
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
	DCache_Invalidate(FLASH_BASE + src, len);

	return 1;
}

int flash_erase_chip()
{
	FLASH_Erase(EraseChip, 0);
	SCB_InvalidateDCache();
	return 1;
}

__attribute__((optimize("O2")))
int crc32_memory_hardware(uint32_t addr, uint32_t len, uint32_t* result)
{
	uint32_t crc = 0xFFFFFFFF;
	const uint8_t* p = (const uint8_t*)addr;
	while(len >= 4)
	{
		crc = (crc >> 8) ^ Crc32Table[(crc ^ *p++) & 0xFF];
		crc = (crc >> 8) ^ Crc32Table[(crc ^ *p++) & 0xFF];
		crc = (crc >> 8) ^ Crc32Table[(crc ^ *p++) & 0xFF];
		crc = (crc >> 8) ^ Crc32Table[(crc ^ *p++) & 0xFF];
		len -= 4;
	}

	while(len--)
		crc = (crc >> 8) ^ Crc32Table[(crc ^ *p++) & 0xFF];

	*result = crc ^ 0xffffffff;
	return 1;
}

int sha256_memory_hardware(uint32_t addr, uint32_t len)
{
	return 0;
}

int read_efuse(void)
{
	EFUSE_LogicalMap_Read((uint8_t*)&cmd_buf);
	return 1024;
}

int read_factory_mac(uint8_t mac[6])
{
	return 0;
}

void get_chip_data(void)
{
	memset(cmd_buf, 0, 16);
	WRITE_REG32(cmd_buf, 0xA8949DBA); // RTL8720D
}

extern int main(void);

__attribute__((used)) void flasher_stub(void)
{
	__asm volatile ("cpsid i" : : : "memory");
	memset((void*)__image1_bss_start__, 0, (__image1_bss_end__ - __image1_bss_start__));
	uint32_t Temp = REG32(SYSTEM_CTRL_BASE_LP + REG_LP_CLK_CTRL0);
	Temp &= ~(BIT_MASK_FLASH_CLK_SEL << BIT_SHIFT_FLASH_CLK_SEL);
	Temp |= BIT_SHIFT_FLASH_CLK_XTAL;
	WRITE_REG32(SYSTEM_CTRL_BASE_LP + REG_LP_CLK_CTRL0, Temp);

	CPU_ClkSet(0);
	DelayUs(10);
	for(uint32_t i = 0; i < 256; i++)
	{
		uint32_t c = i;
		for(int j = 0; j < 8; j++)
		{
			if((c & 1) != 0)
			{
				c = (0xEDB88320 ^ (c >> 1));
			}
			else
			{
				c = c >> 1;
			}
		}
		Crc32Table[i] = c;
	}
	main();
}

__attribute__((section(".image1.entry.data"), used))
RAM_FUNCTION_START_TABLE RamStartTable = {
	.RamStartFun = flasher_stub,
	.RamWakeupFun = flasher_stub,
	.RamPatchFun0 = flasher_stub,
	.RamPatchFun1 = flasher_stub,
	.RamPatchFun2 = flasher_stub,
	.FlashStartFun = flasher_stub,
};
