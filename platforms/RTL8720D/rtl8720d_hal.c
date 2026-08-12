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
	RCC_PeriphClockCmd(0x80100000, 0x40100000, 1); // enable gpio
	flash_rx_mode_switch(0);
	FLASH_RxCmd(flash_init_para.FLASH_cmd_rd_id, 3, (uint8_t*)&flash_id);
	if((flash_id & 0xFF) == 0x20)
	{
		flash_init_para.FLASH_cmd_chip_e = 0xC7;
	}
}

void flash_program_page(uint32_t off, const uint8_t* data)
{
	FLASH_TxData256B(off, FLASH_PAGE_SIZE, (uint8_t*)data);
	DCache_Invalidate(FLASH_BASE + off, FLASH_PAGE_SIZE);
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

int read_efuse(void)
{
	EFUSE_LogicalMap_Read((uint8_t*)&cmd_buf);
	return 1024;
}

static inline uint32_t SYSCFG_CUTVersion_U(void)
{
	uint32_t tmp = (READ_REG32(0x48000000 + 0x000C) >> 8) & 0xF;
	if(0 == tmp) return SYSCFG_CUT_VERSION_A;
	else
	{
		tmp = READ_REG32(0x48000000 + 0x03F0) & 0xF;
		if(0 == tmp) return SYSCFG_CUT_VERSION_B;
		else return tmp;
	}
}

static inline uint32_t SYSCFG_CUTVersion(void)
{
	uint32_t vendortmp = (READ_REG32(0x48000000 + 0x03F0) >> 4) & 0x3;
	uint32_t cuttmp = READ_REG32(0x48000000 + 0x03F0) & 0xF;
	if(0x0 == vendortmp) return SYSCFG_CUTVersion_U();
	else if(0x1 == vendortmp) return cuttmp;
	else return cuttmp;
}

void get_chip_data(void)
{
	WRITE_REG32(cmd_buf, 0xA8949DBA); // RTL8720D
	WRITE_REG32(cmd_buf + 4, SYSCFG_GetChipInfo());
	WRITE_REG32(cmd_buf + 8, SYSCFG_CUTVersion());
	WRITE_REG32(cmd_buf + 12, SYSCFG_ROMINFO_Get());
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
