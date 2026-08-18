#include "../hal_generic.h"

#define UART LOGUART_DEV
const uint16_t Flash_Speed = CLKDIV(2);

#ifndef DISABLE_KV

static bool kv_init = false;
const uint32_t LFS_FLASH_BASE_ADDR = 0x083E0000;
const uint32_t LFS_FLASH_SIZE = 0x20000;
static uint8_t tlsf_area[0x2000];
static tlsf_t memalloc;
lfs_t g_lfs = { 0 };
int lfs_nor_read(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size);

int lfs_nor_prog(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size);

int lfs_nor_erase(const struct lfs_config* c, lfs_block_t block);

int lfs_diskio_sync(const struct lfs_config* c);

struct lfs_config g_nor_lfs_cfg = {
	.read = lfs_nor_read,
	.prog = lfs_nor_prog,
	.erase = lfs_nor_erase,
	.sync = lfs_diskio_sync,

	.read_size = 1,
	.prog_size = 1,
	.block_size = 4096,
	.lookahead_size = 8,
	.cache_size = 256,
	.block_cycles = 100,
};

int lfs_nor_read(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size)
{
	if(size == 0)
	{
		return LFS_ERR_OK;
	}

	memcpy(buffer, (void*)LFS_FLASH_BASE_ADDR + c->block_size * block + off, size);

	return LFS_ERR_OK;
}

int FLASH_WriteStream(uint32_t address, uint32_t len, uint8_t* pbuf)
{
	uint32_t page_begin = address & (~0xff);
	uint32_t page_end = (address + len - 1) & (~0xff);
	uint32_t page_cnt = ((page_end - page_begin) >> 8) + 1;

	uint32_t addr_begin = address;
	uint32_t addr_end = (page_cnt == 1) ? (address + len) : (page_begin + 0x100);
	uint32_t size = addr_end - addr_begin;

	while(page_cnt)
	{
		FLASH_TxData(addr_begin, size, pbuf);
		pbuf += size;

		page_cnt--;
		addr_begin = addr_end;
		addr_end = (page_cnt == 1) ? (address + len) : (addr_begin + 0x100);
		size = addr_end - addr_begin;
	}

	DCache_Invalidate(FLASH_BASE + address, len);

	return 1;
}

int lfs_nor_prog(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size)
{
	if(size == 0)
	{
		return LFS_ERR_OK;
	}

	if((off + size) > c->block_size)
	{
		return LFS_ERR_IO;
	}

	FLASH_WriteStream((LFS_FLASH_BASE_ADDR - FLASH_BASE) + c->block_size * block + off, size, (uint8_t*)buffer);

	return LFS_ERR_OK;
}

int lfs_nor_erase(const struct lfs_config* c, lfs_block_t block)
{
	if(c->block_size != 0x1000)
	{
		return LFS_ERR_IO;
	}

	FLASH_Erase(EraseSector, (LFS_FLASH_BASE_ADDR - FLASH_BASE) + c->block_size * block);
	DCache_Invalidate(FLASH_BASE + LFS_FLASH_BASE_ADDR - FLASH_BASE + c->block_size * block, 0x1000);

	return LFS_ERR_OK;
}

int lfs_diskio_sync(const struct lfs_config* c)
{
	(void)c;
	return LFS_ERR_OK;
}

void* malloc(size_t size)
{
	return tlsf_malloc(memalloc, size);
}

void* realloc(void* ptr, size_t size)
{
	return tlsf_realloc(memalloc, ptr, size);
}

void free(void* ptr)
{
	tlsf_free(memalloc, ptr);
}

void stub_kv_init(void)
{
	if(!kv_init)
	{
		kv_init = true;
		memalloc = tlsf_create_with_pool((void*)&tlsf_area, sizeof(tlsf_area));
		g_nor_lfs_cfg.block_count = LFS_FLASH_SIZE / 4096;
		int ret = lfs_mount(&g_lfs, &g_nor_lfs_cfg);
		if(ret == LFS_ERR_NOTFMT)
		{
			lfs_format(&g_lfs, &g_nor_lfs_cfg);
			lfs_mount(&g_lfs, &g_nor_lfs_cfg);
		}
		rt_kv_init();
	}
}

#endif

void uart_putc(uint8_t b)
{
	while(!(UART->LOGUART_UART_LSR & (1 << 6)));
	UART->LOGUART_UART_THRx[0] = b;
}

int8_t uart_getc_timeout(uint8_t* out, uint32_t loops)
{
	while(loops--)
	{
		if(UART->LOGUART_UART_LSR & 1)
		{
			*out = UART->LOGUART_UART_RBR;
			return 1;
		}
	}
	return 0;
}

void uart_set_baud(uint32_t baud)
{
	LOGUART_SetBaud(UART, baud);
}

void flash_init(void)
{
	RCC_PeriphClockSource_SPIC(0);
	FLASH_DeepPowerDown(0);
	if(SYSCFG_OTP_SPICAddr4ByteEn())
	{
		flash_init_para.FLASH_addr_phase_len = ADDR_4_BYTE;
	}
	if(flash_init_para.FLASH_addr_phase_len == ADDR_4_BYTE)
	{
		FLASH_Addr4ByteEn();
	}
	FLASH_StructInit(&flash_init_para);
	FLASH_RxCmd(flash_init_para.FLASH_cmd_rd_id, 3, (uint8_t*)&flash_id);
}

void flash_program_page(uint32_t off, const uint8_t* data)
{
	FLASH_TxData(off, FLASH_PAGE_SIZE, (uint8_t*)data);
	DCache_Invalidate(FLASH_BASE + off, FLASH_PAGE_SIZE);
	WATCHDOG_REFRESH();
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
			WATCHDOG_REFRESH();
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
	WATCHDOG_REFRESH();
	FLASH_Erase(EraseChip, 0);
	WATCHDOG_REFRESH();
	SCB_InvalidateDCache();
	return 1;
}

__attribute__((optimize("O1")))
int sha256_memory_hardware(uint32_t addr, uint32_t len)
{
	__attribute__((aligned(32))) uint8_t hash[32] = { 0 };
	hw_sha_context ctx = { 0 };
	rtl_crypto_sha2_init(SHA2_256, &ctx);
	uint32_t remaining = len;
	while(remaining > 0)
	{
		uint32_t chunk = remaining > 0x40000 ? 0x40000 : remaining;
		WATCHDOG_REFRESH();
		rtl_crypto_sha2_update((uint8_t*)addr, chunk, &ctx);
		addr += chunk;
		remaining -= chunk;
	}

	rtl_crypto_sha2_final(hash, &ctx);
	memcpy(cmd_buf, hash, 32);
	return 1;
}

int read_efuse(void)
{
	OTP_LogicalMap_Read((uint8_t*)&cmd_buf, 0, 0x400);
	return 0x400;
}

__attribute__((optimize("O1")))
uint32_t hal_read_otp(uint32_t otp_block_size, uint32_t otp_block_count, uint32_t otp_interval, uint32_t otp_start_addr, uint32_t otp_mode)
{
	FLASH_SetSpiMode(&flash_init_para, 0);
	uint32_t out_offset = 0;
	bool any_valid = false;
	for(uint32_t blk = 0; blk < otp_block_count; ++blk)
	{
		uint32_t addr = otp_start_addr + blk * otp_interval;
		uint32_t remain = otp_block_size;

		while(remain > 0)
		{
			uint32_t len = remain > 4 ? 4 : remain;
			uint8_t* dst = cmd_buf + out_offset;
			if(otp_mode)
				FLASH_RxData(0x03, addr, len, dst);
			else
				FLASH_RxData(0x48, addr, len, dst);

			if(!(dst[0] == 0x00 && dst[1] == 0xFF && dst[2] == 0xFF && dst[3] == 0xFF))
			{
				any_valid = true;
			}
			addr += len;
			out_offset += len;
			remain -= len;
		}
	}
	FLASH_SetSpiMode(&flash_init_para, flash_init_para.FLASH_cur_bitmode);
	if(!any_valid)
		return -1;
	return out_offset;
}

void hal_spi_cmd(uint8_t cmd)
{
	FLASH_TxCmd(cmd, 0, 0);
}

void get_chip_data(void)
{
	WRITE_REG32(cmd_buf, 0xF9073AB3); // RTL8721DA
}

void PLL_Cmd(uint32_t status)
{
	PLL_TypeDef* PLL = (PLL_TypeDef*)PLL_BASE;
	if(status == 0)
	{
		PLL->PLL_CTRL0 &= ~(PLL_BIT_REG_CK_EN);
		PLL->PLL_CTRL0 &= ~(PLL_BIT_POW_PLL);
		PLL->PLL_PS &= (~(PLL_BIT_EN_CK400M | PLL_BIT_EN_CK400M_PS));
		PLL->PLL_PS &= ~PLL_BIT_POW_CKGEN;

		PLL->PLL_CTRL0 &= ~PLL_BIT_POW_ERC;
	}
	else
	{
		PLL->PLL_CTRL0 |= PLL_BIT_POW_ERC;
		DelayUs(1);
		PLL->PLL_PS |= PLL_BIT_EN_CK400M | PLL_BIT_EN_CK400M_PS;
		PLL->PLL_PS |= PLL_BIT_POW_CKGEN;
		PLL->PLL_CTRL0 |= PLL_BIT_POW_PLL;
		PLL->PLL_CTRL0 |= PLL_BIT_REG_CK_EN;
		while(!(PLL->PLL_STATE & PLL_BIT_CKRDY_PLL));
	}
}

void PLL_ClkSet(uint32_t PllClk)
{
	PLL_TypeDef* PLL = (PLL_TypeDef*)PLL_BASE;
	uint32_t Div, FoF, FoN, Temp;
	uint32_t XtalClk = XTAL_ClkGet();

	PLL_Cmd(0);

	if(PllClk > 600 * 1000 * 1000)
	{
		PLL->PLL_CTRL0 &= ~PLL_MASK_REG_RS_SET;
		PLL->PLL_CTRL0 |= PLL_REG_RS_SET(PLL_RS_5K);
	}
	else
	{
		PLL->PLL_CTRL0 &= ~PLL_MASK_REG_RS_SET;
		PLL->PLL_CTRL0 |= PLL_REG_RS_SET(PLL_RS_3K_XTAL_26M);
	}

	PLL_Cmd(1);

	Div = PllClk / XtalClk;

	Temp = PllClk - Div * XtalClk;
	FoN = Temp / (XtalClk >> 3);

	Temp = Temp - FoN * (XtalClk >> 3);
	FoF = (uint32_t)(((uint64_t)Temp << 13) / (XtalClk >> 3));

	PLL->PLL_CTRL0 &= ~PLL_BIT_REG_CK_EN;

	Temp = PLL->PLL_CTRL1;
	Temp &= ~PLL_MASK_DIVN_SDM;
	Temp |= PLL_DIVN_SDM(Div - 2);
	PLL->PLL_CTRL1 = Temp;

	Temp = PLL->PLL_CTRL3;
	Temp &= ~(PLL_MASK_F0F_SDM | PLL_MASK_F0N_SDM);
	Temp |= PLL_F0F_SDM(FoF) | PLL_F0N_SDM(FoN);
	PLL->PLL_CTRL3 = Temp;

	DelayUs(1);
	PLL->PLL_CTRL1 |= PLL_BIT_TRIG_FREQ;
	PLL->PLL_CTRL1 &= ~PLL_BIT_TRIG_FREQ;

	DelayUs(60);

	PLL->PLL_CTRL0 |= PLL_BIT_REG_CK_EN;
}

void BOOT_SOC_ClkSet(void)
{
	uint32_t Temp;
	uint32_t PeriDiV, SramDiv, QspiDiv;

	SocClk_Info_TypeDef pSocClk_Info_temp =
	{
		620000000u,
		0,
		CLKDIV(2),
		CLKDIV(6),
		CLKDIV(2)
	};
	SocClk_Info_TypeDef* pSocClk_Info = &pSocClk_Info_temp;
	uint32_t PllClk = pSocClk_Info->PLL_CLK;

	SramDiv = (PllClk / pSocClk_Info->KM4_CKD > SRAM_CLK_LIMIT) ? CLKDIV(2) : CLKDIV(1);
	PeriDiV = CLKDIV_ROUND_UP(PllClk, PERI_CLK_LIMIT);
	QspiDiv = CLKDIV_ROUND_UP(PllClk, QSPI_CLK_LIMIT);
	Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0);
	Temp = Temp & ~LSYS_MASK_CKSL_LP;
	Temp = Temp & ~LSYS_BIT_CKSL_HP;
	Temp = Temp & ~LSYS_BIT_CKSL_HPERI;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0, Temp);
	Temp = CPU_ClkGet();
	DelayClkUpdate(Temp);
	PLL_ClkSet(PllClk);
	Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_GRP0);
	Temp &= ~(LSYS_MASK_CKD_HP | LSYS_MASK_CKD_SRAM);
	Temp &= ~LSYS_MASK_CKD_LP;
	Temp &= ~LSYS_MASK_CKD_HPERI;
	Temp &= ~LSYS_MASK_CKD_QSPI;
	Temp |= LSYS_CKD_HP(pSocClk_Info->KM4_CKD - 1);
	Temp |= LSYS_CKD_LP(pSocClk_Info->KM0_CKD - 1);
	Temp |= LSYS_CKD_SRAM(SramDiv - 1);
	Temp |= LSYS_CKD_HPERI(PeriDiV - 1);
	Temp |= LSYS_CKD_QSPI(QspiDiv - 1);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_GRP0, Temp);
	Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0);
	Temp |= LSYS_BIT_CKSL_HP;
	Temp |= LSYS_BIT_CKSL_HPERI;
	Temp |= LSYS_CKSL_LP(CLK_KM0_PLL);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0, Temp);
	Temp = CPU_ClkGet();
	DelayClkUpdate(Temp);
	PllClk = PLL_ClkGet();
	uint8_t flash_speed = Flash_Speed - 1;
	uint8_t spic_ckd = CLKDIV_ROUND_UP(PllClk, SPIC_CLK_LIMIT) - 1;
	flash_speed = MAX(flash_speed, spic_ckd);
	uint32_t Km4Clk = PllClk / pSocClk_Info->KM4_CKD;
	uint32_t Km0Clk = PllClk / pSocClk_Info->KM0_CKD;
	DiagPrintf("PLL CLK: %d MHz\n", PllClk / 1000 / 1000);
	DiagPrintf("KM4 CPU CLK: %d MHz\n", Km4Clk / 1000 / 1000);
	DiagPrintf("KM0 CPU CLK: %d MHz\n", Km0Clk / 1000 / 1000);
	DiagPrintf("FLASH CLK: %d MHz\n", PllClk / (2 * (flash_speed + 1)) / 1000 / 1000);
}

extern int main(void);

__attribute__((used)) void flasher_stub(void)
{
	WATCHDOG_REFRESH();
	__asm volatile ("cpsid i" : : : "memory");
	extern uint8_t __StackTop[];
	__asm volatile ("msr msp, %0\n" :: "r" (__StackTop));
#if !defined(NO_MINIZ_COMPRESSION) && !defined(NO_MINIZ_DECOMPRESSION)
	BOOT_SOC_ClkSet();
#endif
	memset((void*)__image1_bss_start__, 0, (__image1_bss_end__ - __image1_bss_start__));

	RCC_PeriphClockCmd(APBPeriph_SHA, APBPeriph_SHA_CLOCK, 1);
	RCC_PeriphClockCmd(APBPeriph_LX, APBPeriph_LX_CLOCK, 1);

	CRYPTO_SHA_Init(NULL);

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
