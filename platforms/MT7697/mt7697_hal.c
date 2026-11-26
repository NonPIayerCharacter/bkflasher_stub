#include "../hal_generic.h"

static uint32_t gXtalFreq;
static const uint8_t fraction_L_mapping[10] = { 0x00, 0x10, 0x44, 0x92, 0x29, 0xaa, 0xb6, 0xdb, 0xad, 0xff };
static const uint8_t fraction_M_mapping[10] = { 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 };
static const uint32_t sha256_iv[8] = { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };
uint8_t crypto_input[256] __attribute__((aligned(16)));
uint8_t crypto_output[64] __attribute__((aligned(16)));

void uart_putc(uint8_t b)
{
	while(!(READ_REG32(CM4_UART1_BASE + UART_LSR) & 0x20));
	WRITE_REG32(CM4_UART1_BASE, b);
}

int8_t uart_getc_timeout(uint8_t* out, uint32_t loops)
{
	while(loops--)
	{
		if(READ_REG32(CM4_UART1_BASE + UART_LSR) & 0x1)
		{
			*out = READ_REG32(CM4_UART1_BASE);
			return 1;
		}
	}
	return 0;
}

void uart_set_baud(uint32_t baud)
{
	uint32_t sample_point;
	REG32(CM4_UART1_BASE + UART_RATE_STEP) = 0x3;
	uint32_t uart_lcr = REG32(CM4_UART1_BASE + UART_LCR);
	REG32(CM4_UART1_BASE + UART_LCR) = (uart_lcr | UART_LCR_DLAB);
	uint32_t data = gXtalFreq / baud;
	uint32_t high_speed_div = (data >> 8) + 1;
	uint32_t sample_count = data / high_speed_div - 1;
	if(sample_count == 3)
	{
		sample_point = 0;
	}
	else
	{
		sample_point = sample_count / 2 - 1;
	}
	uint32_t fraction = (gXtalFreq * 10 / baud / data - (sample_count + 1) * 10) % 10;
	REG32(CM4_UART1_BASE + UART_DLL) = (high_speed_div & 0x00ff);
	REG32(CM4_UART1_BASE + UART_DLH) = ((high_speed_div >> 8) & 0x00ff);
	REG32(CM4_UART1_BASE + UART_STEP_COUNT) = sample_count;
	REG32(CM4_UART1_BASE + UART_SAMPLE_COUNT) = sample_point;
	REG32(CM4_UART1_BASE + UART_FRACDIV_M) = fraction_M_mapping[fraction];
	REG32(CM4_UART1_BASE + UART_FRACDIV_L) = fraction_L_mapping[fraction];
	REG32(CM4_UART1_BASE + UART_LCR) = (uart_lcr);
	REG32(CM4_UART1_BASE + UART_FCR) = 0x47;

	uint16_t control_word = REG16(CM4_UART1_BASE + UART_LCR);
	control_word &= ~UART_DATA_MASK;
	control_word |= 3;
	control_word &= ~UART_STOP_MASK;
	control_word |= 0;
	control_word &= ~UART_PARITY_MASK;
	control_word |= 0;
	REG16(CM4_UART1_BASE + UART_LCR) = control_word;

	REG32(CM4_UART1_BASE + UART_VFIFO_EN_REG) |= 0x1;
}

static void sfc_cmd(uint8_t* cmd, uint8_t cmd_len, uint8_t* rx, uint8_t rx_len)
{
	volatile uint32_t* gpram = (volatile uint32_t*)RW_SFC_GPRAM_DATA;
	uint32_t reg;

	if((cmd_len + rx_len) > MAX_SFC_GPRAM_SIZE) return;

	for(uint32_t i = 0; i < (cmd_len + 3) / 4; i++)
	{
		uint32_t word = 0;

		for(uint32_t j = 0; j < 4; j++)
		{
			uint32_t pos = i * 4 + j;

			if(pos < cmd_len) word |= (uint32_t)cmd[pos] << (j * 8);
		}

		gpram[i] = word;
	}

	WRITE_REG32(RW_SFC_MAC_OUTL, cmd_len);
	WRITE_REG32(RW_SFC_MAC_INL, rx_len);

	reg = READ_REG32(RW_SFC_MAC_CTL);
	reg |= SFC_MAC_EN | SFC_TRIG;
	if(READ_REG32(RW_SFC_DIRECT_CTL) & SFC_QPI_EN) reg |= SFC_MAC_XIO_SEL;
	WRITE_REG32(RW_SFC_MAC_CTL, reg);

	while(!(READ_REG32(RW_SFC_MAC_CTL) & SFC_WIP_READY));

	while(READ_REG32(RW_SFC_MAC_CTL) & SFC_WIP);

	reg = READ_REG32(RW_SFC_MAC_CTL);
	reg &= ~(SFC_TRIG | SFC_MAC_EN | SFC_MAC_XIO_SEL | SFC_MAC_SEL);
	WRITE_REG32(RW_SFC_MAC_CTL, reg);

	if(rx && rx_len)
	{
		for(uint32_t i = 0; i < rx_len; i++)
		{
			uint32_t pos = cmd_len + i;
			uint32_t word = gpram[pos / 4];

			rx[i] = (word >> ((pos & 3) * 8)) & 0xFF;
		}
	}

	return;
}

static void flash_wait_ready(void)
{
	uint8_t cmd = 0x05;
	uint8_t sr;

	do
	{
		sfc_cmd(&cmd, 1, &sr, 1);
	} while(sr & 0x01);
}

static void flash_write_enable(void)
{
	uint8_t cmd = 0x06;
	sfc_cmd(&cmd, 1, NULL, 0);
	flash_wait_ready();
}

static void flash_write_disable(void)
{
	uint8_t cmd = 0x04;
	sfc_cmd(&cmd, 1, NULL, 0);
	flash_wait_ready();
}

static void flash_unprotect(void)
{
	uint8_t cmd[2];
	uint8_t sr;
	cmd[0] = 0x05;
	sfc_cmd(cmd, 1, &sr, 1);

	if(sr & 0x1C)
	{
		//sr &= ~(0x1C);
		cmd[0] = 0x01;
		cmd[1] = 0;// sr;

		sfc_cmd(cmd, 2, NULL, 0);
		flash_wait_ready();
	}
}

void flash_program_page(uint32_t off, const uint8_t* data)
{
#define FLASH_WRITE_SIZE 128
	uint8_t cmd[4 + FLASH_WRITE_SIZE];
	uint32_t remaining = FLASH_PAGE_SIZE;
	uint32_t cur = off;
	uint32_t end = off + FLASH_PAGE_SIZE;

	while(remaining)
	{
		uint32_t page_left = FLASH_PAGE_SIZE - (cur % FLASH_PAGE_SIZE);
		uint32_t len = remaining;

		if(len > FLASH_WRITE_SIZE)
			len = FLASH_WRITE_SIZE;

		if(len > page_left)
			len = page_left;

		cmd[0] = 0x02;
		cmd[1] = (cur >> 16) & 0xFF;
		cmd[2] = (cur >> 8) & 0xFF;
		cmd[3] = cur;

		memcpy(cmd + 4, data, len);

		flash_write_enable();

		sfc_cmd(cmd, 4 + len, NULL, 0);

		flash_wait_ready();

		cur += len;
		data += len;
		remaining -= len;
	}
	flash_write_disable();

	cur = off;
	while(cur < end)
	{
		WRITE_REG32(CACHE_OP_BASE, (cur & (0x7FFFFFFUL << 5)));
		OR_REG32(CACHE_OP_BASE, ((2 << 1) | 1));
		cur += 32;
	}
	__asm volatile ("isb");
}

int flash_erase_range(uint32_t src, uint32_t len)
{
	if((src & 0x3) != 0)
		return 0;
	if(len == 0)
		return 0;

	uint32_t cur = src;
	uint32_t end = src + len;
	uint8_t cmd[4];

	while(cur < end)
	{
		if((cur % 0x10000U == 0) && (end - cur >= 0x10000U))
		{
			flash_write_enable();
			flash_wait_ready();
			cmd[0] = 0xD8;
			cmd[1] = (cur >> 16) & 0xFF;
			cmd[2] = (cur >> 8) & 0xFF;
			cmd[3] = cur;
			sfc_cmd(cmd, 4, NULL, 0);
			flash_wait_ready();
			cur += 0x10000U;
		}
		else
		{
			flash_write_enable();
			flash_wait_ready();
			cmd[0] = 0x20;
			cmd[1] = (cur >> 16) & 0xFF;
			cmd[2] = (cur >> 8) & 0xFF;
			cmd[3] = cur;
			sfc_cmd(cmd, 4, NULL, 0);
			flash_wait_ready();
			cur += 0x1000U;
		}
	}
	flash_write_disable();

	cur = src;
	while(cur < end)
	{
		WRITE_REG32(CACHE_OP_BASE, (cur & (0x7FFFFFFUL << 5)));
		OR_REG32(CACHE_OP_BASE, ((2 << 1) | 1));
		cur += 32;
	}
	__asm volatile ("isb");

	return 1;
}

int flash_erase_chip()
{
	flash_write_enable();
	flash_unprotect();
	uint8_t cmd = 0xC7;
	sfc_cmd(&cmd, 1, NULL, 0);
	flash_wait_ready();
	flash_write_disable();
	*CACHE_OP_BASE = CACHE_INVALIDATE_ALL;
	return 1;
}

void flash_init(void)
{
	// SPI mode
	WRITE_REG32(RW_SFC_DIRECT_CTL, 0x10 | 0x0B0B7000);
	uint8_t cmd = 0x9F;
	sfc_cmd(&cmd, 1, (uint8_t*)&flash_id, 3);
}

uint8_t get_chip_data(void)
{
	WRITE_REG32(cmd_buf, 0xDDD6DED1); // MT76x7
	WRITE_REG32(cmd_buf + 4, READ_REG32(0x830081C0U));
	return 8;
}

#ifdef PLATFORM_SUPPORTS_OTP

uint32_t hal_read_otp(uint32_t otp_block_size, uint32_t otp_block_count, uint32_t otp_interval, uint32_t otp_start_addr, uint32_t otp_mode)
{
	uint32_t out_offset = 0;
	for(uint32_t blk = 0; blk < otp_block_count; ++blk)
	{
		uint32_t addr = otp_start_addr + blk * otp_interval;
		uint32_t remain = otp_block_size;

		while(remain > 0)
		{
			uint32_t len = remain > 128 ? 128 : remain;
			uint8_t cmd[4];
			if(otp_mode) cmd[0] = 0x03;
			else cmd[0] = 0x48;
			cmd[1] = addr >> 16;
			cmd[2] = addr >> 8;
			cmd[3] = addr;
			sfc_cmd(cmd, 4, cmd_buf + out_offset, len);
			addr += len;
			out_offset += len;
			remain -= len;
		}
	}
	return out_offset;
}

void hal_spi_cmd(uint8_t cmd)
{
	sfc_cmd(&cmd, 1, NULL, 0);
}

#endif

#ifdef PLATFORM_NO_XIP

void stub_flash_read(void* dest, uint32_t off, size_t len)
{
	uint8_t* p = (uint8_t*)dest;
	uint32_t addr = off;
	uint32_t remain = len;

	while(remain > 0)
	{
		uint32_t chunk = remain > 128 ? 128 : remain;
		uint8_t cmd[5];
		cmd[0] = 0x0B;
		cmd[1] = (uint8_t)(addr >> 16);
		cmd[2] = (uint8_t)(addr >> 8);
		cmd[3] = (uint8_t)addr;
		cmd[4] = 0x00;

		sfc_cmd(cmd, 5, p, chunk);

		addr += chunk;
		p += chunk;
		remain -= chunk;
	}
}

extern uint32_t Crc32Table[256];
__attribute__((optimize("O2")))
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

#endif

int read_efuse(void)
{
	uint32_t* data = (uint32_t*)cmd_buf;
	if(READ_REG32(SYS_EE_CTRL) & SYS_EE_CTRL_EXT_EE_SEL)
		return 0;
	for(uint32_t addr = 0; addr < EFUSE_SIZE; addr += EFUSE_BLOCK_SIZE)
	{
		uint32_t ctrl = READ_REG32(SYS_EFUSE_CTRL);
		ctrl &= ~0x43FF00C0;
		ctrl |= ((addr & 0x3FF) << 16);
		ctrl |= BIT(30);

		WRITE_REG32(SYS_EFUSE_CTRL, ctrl);

		while(READ_REG32(SYS_EFUSE_CTRL) & BIT(30));

		if(!(READ_REG32(SYS_EFUSE_CTRL) & BIT(29)))
		{
			// invalid block
			*data++ = 0xFFFFFFFF;
			*data++ = 0xFFFFFFFF;
			*data++ = 0xFFFFFFFF;
			*data++ = 0xFFFFFFFF;
			continue;
		}

		*data++ = READ_REG32(SYS_EFUSE_RDATA0);
		*data++ = READ_REG32(SYS_EFUSE_RDATA1);
		*data++ = READ_REG32(SYS_EFUSE_RDATA2);
		*data++ = READ_REG32(SYS_EFUSE_RDATA3);
	}
	return EFUSE_SIZE;
}

int sha256_memory_hardware(uint32_t addr, uint32_t len)
{
	const uint8_t* src = (const uint8_t*)addr;
	uint32_t state[8];
	uint32_t remaining = len;
	uint32_t i;
	uint32_t final_stage = 0;
	uint64_t bit_length = (uint64_t)len * 8ULL;

	memcpy(state, sha256_iv, sizeof(state));

	while(1)
	{
		if(remaining >= 64)
		{
			memcpy(crypto_input, src, 64);
			memset(crypto_input + 64, 0, 64);
			src += 64;
			remaining -= 64;
		}
		else if(final_stage == 0)
		{
			memset(crypto_input, 0, sizeof(crypto_input));
			if(remaining != 0) memcpy(crypto_input, src, remaining);
			crypto_input[remaining] = 0x80;

			if(remaining < 56)
			{
				for(i = 0; i < 8; ++i) crypto_input[56 + i] = (uint8_t)(bit_length >> (56 - 8 * i));
				final_stage = 2;
			}
			else final_stage = 1;
			remaining = 0;
		}
		else if(final_stage == 1)
		{
			memset(crypto_input, 0, sizeof(crypto_input));
			for(i = 0; i < 8; ++i) crypto_input[56 + i] = (uint8_t)(bit_length >> (56 - 8 * i));
			final_stage = 2;
		}
		else break;

		memset(crypto_output, 0, sizeof(crypto_output));
		WRITE_REG32(CRYPTO_ENGINE_CTRL, SHA256_ENGINE_CFG);
		WRITE_REG32(CRYPTO_SOURCE_ADDR, crypto_input);
		WRITE_REG32(CRYPTO_DEST_ADDR, crypto_output);
		WRITE_REG32(CRYPTO_TOTAL_LEN, 16);

		for(i = 0; i < 8; ++i) WRITE_REG32(SHA256_IV_BASE + i * 0x10, state[i]);

		WRITE_REG32(SHA256_MODE, 0);
		OR_REG32(SHA256_MODE, CRYPTO_START_BIT);
		OR_REG32(CRYPTO_ENGINE_CTRL, CRYPTO_START_BIT);

		delay_loops(8);

		for(i = 0; i < 8; ++i)
		{
			uint32_t raw;
			memcpy(&raw, crypto_output + i * 4, sizeof(raw));
			state[i] = ((raw & 0x000000ff) << 24) | ((raw & 0x0000ff00) << 8) | ((raw & 0x00ff0000) >> 8) | ((raw & 0xff000000) >> 24);
		}

		REG32(SHA256_MODE) &= ~CRYPTO_START_BIT;
		REG32(CRYPTO_ENGINE_CTRL) &= ~CRYPTO_START_BIT;

		if(final_stage == 2) break;
	}

	memcpy(cmd_buf, crypto_output, 32);
	return 1;
}

void enable_plls(void)
{
	volatile uint32_t reg;
	volatile uint32_t* pTopCfgCM4PWRCtl = (volatile uint32_t*)TOP_CFG_CM4_PWR_CTL_CR;

	reg = READ_REG32(pTopCfgCM4PWRCtl);
	reg = (reg >> CM4_MPLL_EN_SHIFT) & CM4_MPLL_EN_MASK;

	if(reg != CM4_MPLL_EN_PLL1_ON_PLL2_ON)
	{
		reg = READ_REG32(pTopCfgCM4PWRCtl);
		reg = reg & ~(CM4_MCU_960_EN_MASK << CM4_MCU_960_EN_SHIFT);
		reg = reg & ~(CM4_NEED_RESTORE_MASK << CM4_NEED_RESTORE_SHIFT);
		reg = reg | (CM4_MCU_960_EN_DISABLE << CM4_MCU_960_EN_SHIFT);
		WRITE_REG32(pTopCfgCM4PWRCtl, reg);

		reg = READ_REG32(pTopCfgCM4PWRCtl);
		reg = reg & ~(CM4_MPLL_EN_MASK << CM4_MPLL_EN_SHIFT);
		reg = reg & ~(CM4_NEED_RESTORE_MASK << CM4_NEED_RESTORE_SHIFT);
		reg = reg | (CM4_MPLL_EN_PLL1_ON_PLL2_ON << CM4_MPLL_EN_SHIFT);
		WRITE_REG32(pTopCfgCM4PWRCtl, reg);

		do
		{
			reg = READ_REG32(pTopCfgCM4PWRCtl);
			reg = reg & (CM4_WF_PLL_RDY_MASK << CM4_WF_PLL_RDY_SHIFT);
		} while(!reg);

		reg = READ_REG32(pTopCfgCM4PWRCtl);
		reg = reg & ~(CM4_MCU_960_EN_MASK << CM4_MCU_960_EN_SHIFT);
		reg = reg & ~(CM4_NEED_RESTORE_MASK << CM4_NEED_RESTORE_SHIFT);
	}

	reg = READ_REG32(pTopCfgCM4PWRCtl);
	reg = reg | (CM4_MCU_960_EN_ENABLE << CM4_MCU_960_EN_SHIFT);
	WRITE_REG32(pTopCfgCM4PWRCtl, reg);
}

void set_cpu_freq_192mhz(void)
{
	volatile uint32_t reg;
	volatile uint32_t* pTopCfgCM4CKG = (volatile uint32_t*)TOP_CFG_CM4_CKG_EN0;
	enable_plls();
	reg = READ_REG32(pTopCfgCM4CKG);
	reg = reg & ~(CM4_WBTAC_MCU_CK_SEL_MASK << CM4_WBTAC_MCU_CK_SEL_SHIFT);
	reg = reg | (CM4_WBTAC_MCU_CK_SEL_WIFI_PLL_960 << CM4_WBTAC_MCU_CK_SEL_SHIFT);
	WRITE_REG32(pTopCfgCM4CKG, reg);

	while(reg != READ_REG32(pTopCfgCM4CKG));

	reg = READ_REG32(pTopCfgCM4CKG);
	reg = reg & ~(CM4_MCU_DIV_SEL_MASK << CM4_MCU_DIV_SEL_SHIFT);
	reg = reg | (8 << CM4_MCU_DIV_SEL_SHIFT);
	WRITE_REG32(pTopCfgCM4CKG, reg);

	reg = READ_REG32(pTopCfgCM4CKG);
	reg = reg & ~(CM4_HCLK_SEL_MASK << CM4_HCLK_SEL_SHIFT);
	reg = reg | (CM4_HCLK_SEL_PLL << CM4_HCLK_SEL_SHIFT);
	WRITE_REG32(pTopCfgCM4CKG, reg);
}

extern int main(void);
extern uint8_t __StackTop[];

__attribute__((section(".Reset_Handler"), used, naked))
void Reset_Handler(void)
{
	__asm volatile("cpsid i");
#ifdef __ARM_FP
	OR_REG32(0xE000ED88, 0xF << 20);
#endif
	extern uint8_t __bss_start__[];
	extern uint8_t __bss_end__[];
	__asm volatile ("msr msp, %0\n" :: "r" (__StackTop));

	memset((void*)__bss_start__, 0, (__bss_end__ - __bss_start__));

	uint32_t  u4RegVal = 0;
	unsigned long reg = REG32(TOP_AON_CM4_STRAP_STA);
	reg = (reg >> 13) & 0x00000007;

	u4RegVal = REG32(TOP_AON_CM4_PWRCTLCR);
	u4RegVal &= (~(CM4_PWRCTLCR_CM4_XTAL_FREQ_MASK));

	switch(reg)
	{
		case 0:
			gXtalFreq = 20000000;
			u4RegVal |= BIT(CM4_PWRCTLCR_CM4_XTAL_FREQ_20M_OFFSET);
			break;
		case 2:
			gXtalFreq = 26000000;
			u4RegVal |= BIT(CM4_PWRCTLCR_CM4_XTAL_FREQ_26M_OFFSET);
			break;
		case 3:
			gXtalFreq = 52000000;
			u4RegVal |= BIT(CM4_PWRCTLCR_CM4_XTAL_FREQ_52M_OFFSET);
			break;
		default:
			gXtalFreq = 40000000;
			u4RegVal |= BIT(CM4_PWRCTLCR_CM4_XTAL_FREQ_40M_OFFSET);
			break;
	}

	REG32(TOP_AON_CM4_PWRCTLCR) = u4RegVal;

	set_cpu_freq_192mhz();

	// 64mhz flash
	volatile uint32_t* pTopCfgHclk = (volatile uint32_t*)TOP_CFG_HCLK_2M_CKGEN;
	reg = READ_REG32(pTopCfgHclk);
	reg = reg & ~(SF_TOP_CLK_SEL_MASK << SF_TOP_CLK_SEL_SHIFT);
	reg = reg | (SF_TOP_CLK_SEL_SYS_64M << SF_TOP_CLK_SEL_SHIFT);
	WRITE_REG32(pTopCfgHclk, reg);

	mSetHWEntry(SFC_MISC1_DEL_LATCH_LATENCY, 0);
	mSetHWEntry(SFC_MISC1_SMPCK_INV, 1);
	mSetHWEntry(SFC_DELAY_CONTROL3_SFCK_SAM_DLY, 0x18);

	*CACHE_CTRL_BASE = CACHE_DISABLE;
	*CACHE_ENTRY_START = CACHE_START_ADDR;
	*CACHE_ENTRY_END = CACHE_END_ADDR;
	*CACHE_CHANNEL_EN = 1;
	*CACHE_CTRL_BASE = CACHE_ENABLE;
	*CACHE_OP_BASE = CACHE_INVALIDATE_ALL;

	main();
}

void ExpDefHandler(void)
{
	while(1)
	{
		uart_write("ERR", 4);
		delay_loops(500000);
	}
}

__attribute__((section(".vectors"), used)) unsigned long __isr_vector[32] =
{
	(unsigned long)__StackTop,
	(unsigned long)Reset_Handler,
	(unsigned long)ExpDefHandler,
	(unsigned long)ExpDefHandler,
	(unsigned long)ExpDefHandler,
	(unsigned long)ExpDefHandler,
	(unsigned long)ExpDefHandler,
	(unsigned long)0,
	(unsigned long)0,
	(unsigned long)0,
	(unsigned long)0,
	(unsigned long)ExpDefHandler,
	(unsigned long)ExpDefHandler,
	(unsigned long)0,
	(unsigned long)ExpDefHandler,
	(unsigned long)0,
	(unsigned long)ExpDefHandler,
	(unsigned long)ExpDefHandler,
};