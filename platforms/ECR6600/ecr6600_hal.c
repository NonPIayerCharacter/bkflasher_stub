#include "../hal_generic.h"

unsigned int g_efuse_ctrl = 0;
extern E_SPI_MODE spi_mode;

void uart_putc(uint8_t b)
{
	WRITE_REG32(UARTC_THR_REG, b);
	while(!(REG32(UARTC_LSR_REG) & UARTC_LSR_TEMT));
}

int8_t uart_getc_timeout(uint8_t* out, uint32_t loops)
{
	while(loops--)
	{
		if(uart_data_tstc(SOC_UART0_BASE) != 0)
		{
			uart_getc(SOC_UART0_BASE, out, 1);
			return 1;
		}
	}
	return 0;
}

void uart_set_baud(uint32_t baud)
{
	uint32_t uclk;
	uint32_t value = REG32(UARTC_LCR_REG);

	uclk = (REG32(SOC_PD_UART_CLK_SEL) == 0) ? 40000000 : 80000000;

	if(uclk == 40000000)
	{
		int crystal = g_efuse_ctrl;
		if((crystal & EFUSE_CRYSTAL_OFFSET) == EFUSE_CRYSTAL_26M)
			uclk = 26000000;
	}

	uint32_t bestOsc = 0;
	uint32_t bestDiv = 0;

	for(uint32_t osc = 32; osc > 0; osc--)
	{
		uint32_t div = uclk / (osc * baud);

		if(div == 0) continue;

		for(int32_t d = -1; d <= 1; d++)
		{
			uint32_t testDiv = div + d;
			if(testDiv == 0) continue;

			uint32_t actual = uclk / (osc * testDiv);
			uint32_t err = (actual > baud) ? actual - baud : baud - actual;

			if(err * 100 < baud * 5)
			{
				bestOsc = osc;
				bestDiv = testDiv;
				goto done;
			}
		}
	}

done:
	WRITE_REG32(UARTC_OSCR_REG, bestOsc);
	WRITE_REG32(UARTC_LCR_REG, value | 0x80);
	WRITE_REG32(UARTC_DLL_REG, bestDiv & 0xff);
	WRITE_REG32(UARTC_DLM_REG, (bestDiv >> 8) & 0xff);
	WRITE_REG32(UARTC_LCR_REG, value & ~0x80);
}

void uart_init(void)
{
	//uart_init_ecr(SOC_UART0_BASE);
	uart_set_baud(115200U);
}

void flash_program_page(uint32_t off, const uint8_t* data)
{
	if(spi_mode == SPI_MODE_QUAD)
	{
		flash_quad_page_program(off, data, FLASH_PAGE_SIZE);
	}
	else
	{
		flash_page_program(off, data, FLASH_PAGE_SIZE);
	}
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
			flash_erase_block(cur);
			cur += 0x10000U;
		}
		else
		{
			flash_erase_sector(cur);
			cur += 0x1000U;
		}
	}

	return 1;
}

int flash_erase_chip()
{
	flash_chip_erase();
	return 1;
}

void flash_init(void)
{
	WRITE_REG32(SOC_PD_SPI_CONFIG, REG32(SOC_PD_SPI_CONFIG) | 0x80);
	flash_init_ecr(0xFF, 5);
}

int crc32_memory_hardware(uint32_t addr, uint32_t len, uint32_t* result)
{
	*result = crc32(0, (void*)addr, len);
	return 1;
}

int read_factory_mac(uint8_t mac[6])
{
	return 0;
}

int read_efuse(void)
{
	efuse_read_series(0, (unsigned int*)cmd_buf, 0x80 / 4);
	return 0x80;
}

int sha256_memory_hardware(uint32_t addr, uint32_t len)
{
	sha256((void*)addr, len, cmd_buf);
	return 1;
}

void boot_soc_update_cpu_clk(void)
{
	unsigned int val;
	g_efuse_ctrl = efuse_read(EFUSE_CTRL_OFFSET);
	OR_REG32(SOC_PD_CLK_EN_BASE0, RF_REGF_APB_CLK_BIT | FREF_CLK_EN_BIT);

	if((g_efuse_ctrl & EFUSE_CRYSTAL_OFFSET) == EFUSE_CRYSTAL_40M)
	{
		val = READ_REG32(SOC_RFC_DIG_PLL3);
		val = (val & 0xFFFFFF0F) | 0x60;
		WRITE_REG32(SOC_RFC_DIG_PLL3, val);
	}
	else
	{
		val = READ_REG32(SOC_RFC_DIG_PLL3);
		val = (val & 0xFFFF7F0F) | 0x60;
		WRITE_REG32(SOC_RFC_DIG_PLL3, val);
		val = READ_REG32(SOC_RFC_DIG_PLL2);
		val = (val & (~(0x7F << 14))) | (0x25 << 14);
		WRITE_REG32(SOC_RFC_DIG_PLL2, val);
		WRITE_REG32(SOC_DIG_PLL_REG13, 0x49);
		WRITE_REG32(SOC_DIG_PLL_REG14, 0x6C4EC5);
		WRITE_REG32(SOC_DIG_PLL_REG20, 0x49E);
	}

	val = READ_REG32(SOC_RFC_DIG_PLL1);
	val |= 1 << 13;
	WRITE_REG32(SOC_RFC_DIG_PLL1, val);

	udelay(10);

	WRITE_REG32(SOC_DIG_PLL_REG18, 1);

	udelay(10);

	val = READ_REG32(SOC_RFC_DIG_PLL3);
	val &= ~((1 << 14) | 0xF);
	WRITE_REG32(SOC_RFC_DIG_PLL3, val);

	WRITE_REG32(SOC_DIG_PLL_REG18, 0);

	udelay(5);

	val = READ_REG32(SOC_RFC_DIG_PLL2);
	val |= 1 << 27;
	WRITE_REG32(SOC_RFC_DIG_PLL2, val);

	val = READ_REG32(SOC_DIG_PLL_REG12);
	val |= 1;
	WRITE_REG32(SOC_DIG_PLL_REG12, val);

	udelay(20);

	val = READ_REG32(SOC_RFC_DIG_PLL3);
	val |= 1 << 23;
	WRITE_REG32(SOC_RFC_DIG_PLL3, val);

	WRITE_REG32(0x00202074, 0x6);

	udelay(1);

	efuse_cfg_160m();
	WRITE_REG32(SOC_PD_CLK_MUX_BASE, (READ_REG32(SOC_PD_CLK_MUX_BASE) & 0xFFFFFFF0) | CPU_CLK_FREQ_DIV0 | CPU_CLK_SEC_160M);
	WRITE_REG32(SOC_PD_UART_CLK_SEL, UART_CLK_SEL_160M_DIV2);
	udelay(1);
}

extern int main(void);

int boot_main(void)
{
	extern uint8_t __bss_start__[];
	extern uint8_t __bss_end__[];
	memset((void*)__bss_start__, 0, (__bss_end__ - __bss_start__));
	boot_soc_update_cpu_clk();
	main();
}
