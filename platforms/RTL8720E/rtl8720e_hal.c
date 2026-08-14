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
	RCC_PeriphClockSource_SPIC(2);
	FLASH_DeepPowerDown(0);
	if(SYSCFG_OTP_SPICAddr4ByteEn())
	{
		flash_init_para.FLASH_addr_phase_len = ADDR_4_BYTE;
	}
	FLASH_StructInit(&flash_init_para);
	FLASH_RxCmd(flash_init_para.FLASH_cmd_rd_id, 3, (uint8_t*)&flash_id);

#ifndef DISABLE_KV
	g_nor_lfs_cfg.block_count = LFS_FLASH_SIZE / 4096;
	int ret = lfs_mount(&g_lfs, &g_nor_lfs_cfg);
	if(ret == LFS_ERR_NOTFMT)
	{
		lfs_format(&g_lfs, &g_nor_lfs_cfg);
		lfs_mount(&g_lfs, &g_nor_lfs_cfg);
	}
	rt_kv_init();
#endif
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

static void OTP_PowerCmd(bool Enable)
{
	uint32_t Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_PWC);

	if(Enable)
	{
		Temp |= AON_BIT_PWC_AON_OTP;
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_PWC, Temp);
	}
	else
	{
		Temp &= ~(AON_BIT_PWC_AON_OTP);
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_PWC, Temp);
	}
}

static bool OTP_GetPowerState(void)
{
	uint32_t Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_PWC);

	if(Temp & AON_BIT_PWC_AON_OTP)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

static void OTP_AccessCmd(bool Enable)
{
	OTPC_TypeDef* OTPC = OTPC_DEV;

	if(Enable)
	{
		OTPC->OTPC_OTP_CTRL |= OTPC_EF_PG_PWD(OTP_ACCESS_PWD);
	}
	else
	{
		OTPC->OTPC_OTP_CTRL &= ~OTPC_MASK_EF_PG_PWD;
	}
}

static void OTPPowerSwitch(bool BWrite, bool PwrState)
{
	if(PwrState == 1)
	{
		if(OTP_GetPowerState() == 0)
		{
			OTP_PowerCmd(1);
		}
	}

	OTP_AccessCmd(BWrite);
}

int OTP_Read8(uint32_t Addr, uint8_t* Data)
{
	uint32_t TmpIdx = 0;
	int bResult = -1;
	volatile uint32_t* AccessReg = NULL;
	OTPC_TypeDef* OTPC = OTPC_DEV;

	if(Addr >= OTP_REAL_CONTENT_LEN)
	{
		*Data = 0xff;
		return bResult;
	}

	AccessReg = &(OTPC->OTPC_OTP_AS);

	OTPPowerSwitch(0, 1);

	*AccessReg = OTPC_EF_ADDR_NS(Addr);

	while((!(OTPC_BIT_EF_RD_WR_NS & (*AccessReg))) && (TmpIdx < OTP_POLL_TIMES))
	{
		DelayUs(5);
		TmpIdx++;
	}

	if(TmpIdx < OTP_POLL_TIMES)
	{
		*Data = (uint8_t)((*AccessReg) & OTPC_MASK_EF_DATA_NS);
		bResult = 0;
	}
	else
	{
		*Data = 0xff;
		bResult = -1;
	}

	OTPPowerSwitch(0, 0);

	return bResult;
}

static int OTP_Read32(uint32_t Addr, uint32_t* Data)
{
	uint8_t Temp;
	uint8_t i;

	*Data = 0;

	for(i = 0; i < 4; i++)
	{
		if(OTP_Read8(Addr++, &Temp) == -1)
		{
			goto exit;
		}

		*Data |= (((uint32_t)Temp) << (8 * i));
	}

	return 0;
exit:
	return -1;
}

int OTP_LogicalMap_Read(uint8_t* pbuf, uint32_t addr, uint32_t len)
{
	uint32_t OTP_Addr = 0;
	uint32_t offset;
	uint32_t OTPData;
	uint8_t data, plen, type;

	if((addr + len) > OTP_LMAP_LEN)
	{
		return -1;
	}

	memset(pbuf, 0xFF, len);

	while(OTP_Addr < LOGICAL_MAP_SECTION_LEN)
	{
		OTP_Read32(OTP_Addr, &OTPData);

		if(OTPData == 0xFFFFFFFF)
		{
			break;
		}

		OTP_Addr += 4;

		switch(OTP_GET_LTYP(OTPData))
		{
			case OTP_LTYP0:
				break;

			case OTP_LTYP1:
				if(OTP_GET_LTYP1_BASE(OTPData) == OTP_LBASE_EFUSE)
				{
					offset = OTP_GET_LTYP1_OFFSET(OTPData);
					data = OTP_GET_LTYP1_DATA(OTPData);

					if((offset >= addr) && (offset < addr + len))
					{
						pbuf[offset - addr] = data;
					}
				}

				break;

			case OTP_LTYP2:
				plen = OTP_GET_LTYP2_LEN(OTPData);
				offset = OTP_GET_LTYP2_OFFSET(OTPData);
				type = OTP_GET_LTYP2_BASE(OTPData);

				if(type == OTP_LBASE_EFUSE)
				{
					while(plen-- > 0)
					{
						OTP_Read8(OTP_Addr++, &data);

						if((offset >= addr) && (offset < addr + len))
						{
							pbuf[offset - addr] = data;
						}
						offset++;
					}
				}
				else
				{
					OTP_Addr += plen;
				}

				break;

			default:
				break;

		}
	}

	return 0;
}

int read_efuse(void)
{
	OTP_LogicalMap_Read((uint8_t*)&cmd_buf, 0, 0x400);
	return 0x400;
}

void get_chip_data(void)
{
	WRITE_REG32(cmd_buf, 0xDF93AD2C); // RTL8720E
}

void CPU_ClkSet_NonOS(uint32_t Source)
{
	uint32_t Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0);

	/* 1. Select CPU clock source */
	if(Source != LSYS_GET_CKSL_PLFM(Temp))
	{
		Temp &= ~(LSYS_MASK_CKSL_PLFM);
		Temp |= LSYS_CKSL_PLFM(Source);
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0, Temp);

		DSB();
		delay_loops(1);
	}

	/* 2. update for DelayUs */
	RBSS_UDELAY_CLK = CPU_ClkGet() / MHZ_TICK_CNT;
}

void PLL_ClkSet(uint8_t pll_type, uint32_t PllClk)
{
	PLL_TypeDef* PLL = (PLL_TypeDef*)PLL_BASE;
	uint32_t Div, FoF, FoN, Temp;
	uint32_t XtalClk = XTAL_ClkGet();

	//PllClk = Div * XtalClk + (FoN + FoF >> 13) / 8 * XtalClk;
	Div = PllClk / XtalClk;

	Temp = PllClk - Div * XtalClk; //FoN represents [0, XtalClk)
	FoN = Temp / (XtalClk >> 3);

	Temp = Temp - FoN * (XtalClk >> 3); //FoF represents [0, XtalClk/8)
	FoF = (uint32_t)(((uint64_t)Temp << 13) / (XtalClk >> 3));

	if(pll_type == CLK_CPU_MPLL)
	{
		Temp = PLL->PLL_CPUPLL_CTRL1;
		Temp &= ~PLL_MASK_CPUPLL_DIVN_SDM;
		Temp |= PLL_CPUPLL_DIVN_SDM(Div - 2);
		PLL->PLL_CPUPLL_CTRL1 = Temp;

		Temp = PLL->PLL_CPUPLL_CTRL3;
		Temp &= ~(PLL_MASK_CPUPLL_F0F_SDM | PLL_MASK_CPUPLL_F0N_SDM);
		Temp |= PLL_CPUPLL_F0F_SDM(FoF) | PLL_CPUPLL_F0N_SDM(FoN);
		PLL->PLL_CPUPLL_CTRL3 = Temp;

		DelayUs(1);
		PLL->PLL_CPUPLL_CTRL1 |= PLL_BIT_CPUPLL_TRIG_FREQ;
		PLL->PLL_CPUPLL_CTRL1 &= ~PLL_BIT_CPUPLL_TRIG_FREQ;
		DelayUs(30);
	}
	else if(pll_type == CLK_CPU_DPLL)
	{
		Temp = PLL->PLL_PERIPLL_CTRL1;
		Temp &= ~PLL_MASK_PERIPLL_DIVN_SDM;
		Temp |= PLL_PERIPLL_DIVN_SDM(Div - 2);
		PLL->PLL_PERIPLL_CTRL1 = Temp;

		Temp = PLL->PLL_PERIPLL_CTRL3;
		Temp &= ~(PLL_MASK_PERIPLL_F0F_SDM | PLL_MASK_PERIPLL_F0N_SDM);
		Temp |= PLL_PERIPLL_F0F_SDM(FoF) | PLL_PERIPLL_F0N_SDM(FoN);
		PLL->PLL_PERIPLL_CTRL3 = Temp;

		DelayUs(1);
		PLL->PLL_PERIPLL_CTRL1 |= PLL_BIT_PERIPLL_TRIG_FREQ;
		PLL->PLL_PERIPLL_CTRL1 &= ~PLL_BIT_PERIPLL_TRIG_FREQ;
		DelayUs(30);
	}
	else
	{
		/* error param return */
		return;
	}
}

void BOOT_SOC_ClkSet(void)
{
	uint32_t Temp;
	uint32_t PllMClk, PllDClk, PllClk;
	uint32_t HbusDiV, EcdsaDiv, CpuDiv, SramDiv;
	SocClk_Info_TypeDef* pSocClk_Info;
	//RRAM_TypeDef* rram = RRAM_DEV;

	//uint32_t Boot_Clk_Config_Level = 0;
	//pSocClk_Info = &SocClk_Info[Boot_Clk_Config_Level];

	SocClk_Info_TypeDef pSocClk_Info_temp =
	{
		620000000u,
		500000000u,
		0,
		CLKDIV(2) | ISPLLM,
		CLKDIV(2) | ISPLLM
	};
	pSocClk_Info = &pSocClk_Info_temp;

	PllMClk = pSocClk_Info->PLLM_CLK;
	PllDClk = pSocClk_Info->PLLD_CLK;
	CpuDiv = pSocClk_Info->CPU_CKD & ~ISPLLD;

	if(pSocClk_Info->CPU_CKD & ISPLLD)
	{
		PllClk = PllDClk;
	}
	else
	{
		PllClk = PllMClk;
	}

	/* Note: for sram, max 240MHz under 1.0v, max 200MHz under 0.9v, and km4:Sram shall be 1:1 or 2:1 */
	if(pSocClk_Info->Vol_Type == 0)
	{
		HbusDiV = CLKDIV_ROUND_UP(PllClk, HBUS_0P9V_CLK_LIMIT);
		if(PllClk / CpuDiv > SRAM_0P9V_CLK_LIMIT)
		{
			SramDiv = CpuDiv * 2;
		}
		else
		{
			SramDiv = CpuDiv;
		}
	}
	else
	{
		HbusDiV = CLKDIV_ROUND_UP(PllClk, HBUS_1P0V_CLK_LIMIT);
		if(PllClk / CpuDiv > SRAM_1P0V_CLK_LIMIT)
		{
			SramDiv = CpuDiv * 2;
		}
		else
		{
			SramDiv = CpuDiv;
		}
	}

	EcdsaDiv = CLKDIV_ROUND_UP(PllMClk, ECDSA_CLK_LIMIT);/*target clk ECDSA_CLK_LIMIT*/

	//0. configure core power according user setting
	//SWR_CORE_Vol_Set(pSocClk_Info->Vol_Type);

	//1. select clk_lbus
	CPU_ClkSet_NonOS(0);

	//2. gated selected PLL clock output (1-dsp_pll; 0-cpu_pll)
	PLL_TypeDef* PLL = (PLL_TypeDef*)PLL_BASE;
	PLL->PLL_CPUPLL_CTRL0 &= ~PLL_BIT_CPUPLL_CK_EN;
	PLL->PLL_PERIPLL_CTRL0 &= ~PLL_BIT_PERIPLL_CK_EN;

	//3. modify PLL clock
	PLL_ClkSet(CLK_CPU_MPLL, PllMClk);
	PLL_ClkSet(CLK_CPU_DPLL, PllDClk);
	//rram->PLLD_Clk_Info = PllDClk;

	//4. Disable km4/kr4/sram divn
	Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKE_GRP1);
	Temp &= ~APBPeriph_PLL_CLOCK;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKE_GRP1, Temp);

	//5. enable DSP/CPU PLL clock output (1-dsp_pll; 0-cpu_pll)
	PLL->PLL_CPUPLL_CTRL0 |= PLL_BIT_CPUPLL_CK_EN;
	PLL->PLL_PERIPLL_CTRL0 |= PLL_BIT_PERIPLL_CK_EN;

	//6. change ckd of km4/kr4/sram according to voltage
	Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_GRP0);
	Temp &= ~(LSYS_MASK_CKD_KM4 | LSYS_MASK_CKD_KR4 | LSYS_MASK_CKD_SRAM);
	Temp &= ~(LSYS_MASK_CKD_HBUS | LSYS_MASK_CKD_GDMA_AXI);
	Temp &= ~LSYS_MASK_CKD_PSRAM;

	Temp |= LSYS_CKD_KM4(CpuDiv - 1) | LSYS_CKD_KR4(CpuDiv - 1) | LSYS_CKD_SRAM(SramDiv - 1);
	Temp |= LSYS_CKD_HBUS(HbusDiV - 1) | LSYS_CKD_GDMA_AXI(HbusDiV - 1);
	Temp |= LSYS_CKD_PSRAM(pSocClk_Info->PSRAMC_CKD - 1);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_GRP0, Temp);

	if(pSocClk_Info->PSRAMC_CKD & ISPLLD)
	{
		RCC_PeriphClockSource_PSRAM(BIT_LSYS_CKSL_PSRAM_DSPPLL);
	}
	else
	{
		RCC_PeriphClockSource_PSRAM(BIT_LSYS_CKSL_PSRAM_CPUPLL);
	}

	/* change ckd of ecdsa */
	Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_GRP1);
	Temp = (Temp & ~LSYS_MASK_CKD_ECDSA) | LSYS_CKD_ECDSA(EcdsaDiv - 1);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_GRP1, Temp);

	//7. enable km4/kr4/sram divn
	Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKE_GRP1);
	Temp |= APBPeriph_PLL_CLOCK;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKE_GRP1, Temp);

	//8. select divn_clk
	if(pSocClk_Info->CPU_CKD & ISPLLD)
	{
		CPU_ClkSet_NonOS(CLK_CPU_DPLL);
	}
	else
	{
		CPU_ClkSet_NonOS(CLK_CPU_MPLL);
	}

	PllMClk = PLL_ClkGet(CLK_CPU_MPLL);
	PllDClk = PLL_ClkGet(CLK_CPU_DPLL);
	uint8_t flash_speed = FLASH_CLK_DIV10 - 8;
	uint8_t spic_ckd = CLKDIV_ROUND_UP(PllMClk, SPIC_CLK_LIMIT) - 1;
	flash_speed = MAX(flash_speed, spic_ckd);
	DiagPrintf("PLLM CLK: %d MHz\n", PllMClk / 1000 / 1000);
	DiagPrintf("PLLD CLK: %d MHz\n", PllDClk / 1000 / 1000);
	DiagPrintf("KM4 CPU CLK: %d MHz\n", pSocClk_Info->CPU_CKD & ISPLLD ? PllDClk / CpuDiv / 1000 / 1000 : PllMClk / CpuDiv / 1000 / 1000);
	DiagPrintf("FLASH CLK: %d MHz\n", PllMClk / (2 * (flash_speed + 1)) / 1000 / 1000);
}

extern int main(void);

__attribute__((used)) void flasher_stub(void)
{
	WATCHDOG_REFRESH();
	__asm volatile ("cpsid i" : : : "memory");
	extern uint8_t __StackTop[];
	__asm volatile ("msr msp, %0\n" :: "r" (__StackTop));
	BOOT_SOC_ClkSet();
	memset((void*)__image1_bss_start__, 0, (__image1_bss_end__ - __image1_bss_start__));
	RCC_PeriphClockCmd(APBPeriph_SHA, APBPeriph_SHA_CLOCK, 1);
	RCC_PeriphClockCmd(APBPeriph_LX, APBPeriph_LX_CLOCK, 1);

	CRYPTO_SHA_Init(NULL);

#ifndef DISABLE_KV
	memalloc = tlsf_create_with_pool((void*)&tlsf_area, sizeof(tlsf_area));
#endif

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
