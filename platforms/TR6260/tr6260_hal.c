#include "../hal_generic.h"

E_SPI_MODE spi_mode = SPI_MODE_STANDARD;

static int spi_cmd_none(uint8_t cmd)
{
	if(!spi_bus_ready(SOC_SPI0_BASE)) return -1;
	if(spi_fifo_reset(SOC_SPI0_BASE)) return -2;

	WRITE_REG32(SOC_SPI0_BASE + 0x20, SPI_TRANSCTRL_CMD_EN | SPI_TRANSCTRL_TRAMODE_NONE);
	WRITE_REG32(SOC_SPI0_BASE + 0x24, cmd);

	return 0;
}

static uint32_t spi_read_cmd(uint8_t cmd, uint32_t len)
{
	if(!spi_bus_ready(SOC_SPI0_BASE)) goto fail;
	if(spi_fifo_reset(SOC_SPI0_BASE)) goto fail;
	WRITE_REG32(SOC_SPI0_BASE + 0x20, SPI_TRANSCTRL_CMD_EN | SPI_TRANSCTRL_TRAMODE_RO | SPI_TRANSCTRL_RCNT(len - 1));
	WRITE_REG32(SOC_SPI0_BASE + 0x24, cmd);
	if(!spi_rx_ready(SOC_SPI0_BASE))  goto fail;
	return READ_REG32(SOC_SPI0_BASE + 0x2c);
fail:
	return -1;
}

static void flash_write_enable(void)
{
	while(1)
	{
		spi_cmd_none(SPIFLASH_CMD_WREN);
		if(spi_read_cmd(SPIFLASH_CMD_RDSR_1, 1) & SPIFLASH_STATUS_WEL) break;
	}
}

static uint8_t flash_status(void)
{
	return (uint8_t)spi_read_cmd(SPIFLASH_CMD_RDSR_1, 1);
}

static void flash_wait_busy(void)
{
	while(flash_status() & SPIFLASH_STATUS_WIP);
}

static void flash_write_status(uint32_t cmd, uint32_t value, int len)
{
	flash_write_enable();
	WRITE_REG32(SOC_SPI0_BASE + 0x20, SPI_TRANSCTRL_CMD_EN | SPI_TRANSCTRL_TRAMODE_WO | SPI_TRANSCTRL_WCNT(len - 1));
	WRITE_REG32(SOC_SPI0_BASE + 0x24, cmd);
	WRITE_REG32(SOC_SPI0_BASE + 0x2c, value);
	flash_wait_busy();
}

static int flash_erase_cmd(uint32_t addr, uint8_t cmd)
{
	flash_write_enable();
	if(!spi_bus_ready(SOC_SPI0_BASE)) return -1;
	if(spi_fifo_reset(SOC_SPI0_BASE)) return -2;
	WRITE_REG32(SOC_SPI0_BASE + 0x28, addr);
	WRITE_REG32(SOC_SPI0_BASE + 0x20, SPI_TRANSCTRL_CMD_EN | SPI_TRANSCTRL_ADDR_EN | SPI_TRANSCTRL_TRAMODE_NONE);
	WRITE_REG32(SOC_SPI0_BASE + 0x24, cmd);
	flash_wait_busy();
	return 0;
}

static int spi_otp_read_region(uint32_t cmd, uint32_t addr, uint32_t len, uint8_t* pdata)
{
	if(!spi_bus_ready(SOC_SPI0_BASE))
		return -1;

	if(spi_fifo_reset(SOC_SPI0_BASE))
		return -2;

	WRITE_REG32(SOC_SPI0_BASE + 0x20,
		SPI_TRANSCTRL_CMD_EN |
		SPI_TRANSCTRL_ADDR_EN |
		SPI_TRANSCTRL_TRAMODE_RO |
		SPI_TRANSCTRL_DUALQUAD_REGULAR |
		SPI_TRANSCTRL_RCNT(len - 1));

	WRITE_REG32(SOC_SPI0_BASE + 0x28, addr);
	WRITE_REG32(SOC_SPI0_BASE + 0x24, SPIFLASH_CMD_OTP_RD);

	uint32_t words = len / 4;
	uint32_t* dst = (uint32_t*)pdata;

	for(uint32_t i = 0; i < words; i++)
	{
		while(READ_REG32(SOC_SPI0_BASE + 0x34) & SPI_STATUS_RXENPTY);

		dst[i] = READ_REG32(SOC_SPI0_BASE + 0x2c);
	}

	return 0;
}

void uart_set_baud(uint32_t baud)
{
	uint32_t uclk;
	uint32_t value = READ_REG32(UART0_BASE + 0x2c);

	uclk = 40000000;

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
	WRITE_REG32(UART0_BASE + 0x14, bestOsc);
	WRITE_REG32(UART0_BASE + 0x2c, value | 0x80);
	WRITE_REG32(UART0_BASE + 0x20, bestDiv & 0xff);
	WRITE_REG32(UART0_BASE + 0x24, (bestDiv >> 8) & 0xff);
	WRITE_REG32(UART0_BASE + 0x2c, value & ~0x80);
}

void uart_putc(uint8_t b)
{
	WRITE_REG32(UART0_BASE + 0x20, b);
	while(!(READ_REG32(UART0_BASE + 0x34) & 0x40));
}

int8_t uart_getc_timeout(uint8_t* out, uint32_t loops)
{
	while(loops--)
	{
		if(uart_data_tstc(UART0_BASE))
		{
			*out = uart_data_getc(UART0_BASE);
			return 1;
		}
	}
	return 0;
}

void nds32_dcache_invalidate(void)
{
	__nds32__cctl_l1d_invalall();
	__nds32__msync_store();
	__nds32__dsb();
}

void flash_program_page(uint32_t off, const uint8_t* data)
{
	flash_write_enable();

	if(!spi_bus_ready(SOC_SPI0_BASE)) return;
	if(spi_fifo_reset(SOC_SPI0_BASE)) return;

	if(spi_mode == SPI_MODE_QUAD)
	{
		WRITE_REG32(SOC_SPI0_BASE + 0x20, SPI_TRANSCTRL_CMD_EN | SPI_TRANSCTRL_ADDR_EN | SPI_TRANSCTRL_TRAMODE_WO | SPI_TRANSCTRL_DUALQUAD_QUAD | SPI_TRANSCTRL_WCNT(FLASH_PAGE_SIZE - 1));
		WRITE_REG32(SOC_SPI0_BASE + 0x24, SPIFLASH_CMD_QUAD_INPUT_PP);
	}
	else
	{
		WRITE_REG32(SOC_SPI0_BASE + 0x20, SPI_TRANSCTRL_CMD_EN | SPI_TRANSCTRL_ADDR_EN | SPI_TRANSCTRL_TRAMODE_WO | SPI_TRANSCTRL_WCNT(FLASH_PAGE_SIZE - 1));
		WRITE_REG32(SOC_SPI0_BASE + 0x24, SPIFLASH_CMD_PP);
	}

	WRITE_REG32(SOC_SPI0_BASE + 0x28, off);

	uint32_t words = (FLASH_PAGE_SIZE + 3) / 4;

	for(uint32_t i = 0; i < words; i++)
	{
		while(READ_REG32(SOC_SPI0_BASE + 0x34) & SPI_STATUS_TXFULL);
		WRITE_REG32(SOC_SPI0_BASE + 0x2c, ((uint32_t*)data)[i]);
	}

	flash_wait_busy();
	nds32_dcache_invalidate();
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
			flash_erase_cmd(cur, SPIFLASH_CMD_BE);
			cur += 0x10000U;
		}
		else
		{
			flash_erase_cmd(cur, SPIFLASH_CMD_SE);
			cur += 0x1000U;
		}
	}

	nds32_dcache_invalidate();

	return 1;
}

int flash_erase_chip()
{
	flash_write_enable();
	spi_cmd_none(SPIFLASH_CMD_CE);
	flash_wait_busy();
	nds32_dcache_invalidate();
	return 1;
}

void flash_init(void)
{
	spiFlash_init(0xFF, 5);

	flash_id = spi_read_cmd(SPIFLASH_CMD_RDID, 3) & 0xFFFFFF;

	if(flash_id != 0xFFFFFF)
	{
		switch(flash_id)
		{
			case 0x16701c:
			case 0x16301c:
			case 0x15701c:
				break;
			case 0x146085:
			case 0x1440c8:
			case 0x1540c8:
			case 0x15400b:
			case 0x14400b:
				flash_write_status(SPIFLASH_CMD_WRSR, SPIFLASH_STATUS_QE, 2);
				break;
			case 0x1560eb:
				flash_write_status(SPIFLASH_CMD_WRSR, 0x0218, 2);
				break;
			case 0x156085:
				flash_write_status(SPIFLASH_CMD_WRSR, SPIFLASH_STATUS_QE, 2);
				flash_write_status(SPIFLASH_CMD_WRSR_2, SPIFLASH_STATUS_2_QE, 1);
				break;
			case 0x1820c2:
			case 0x1720c2:
				flash_write_status(SPIFLASH_CMD_WRSR, 0x40, 2);
				break;
			default:
				flash_write_status(SPIFLASH_CMD_WRSR_2, SPIFLASH_STATUS_2_QE, 1);
				break;
		}
		spi_mode = SPI_MODE_QUAD;
	}
	else
	{
		spi_mode = SPI_MODE_STANDARD;
	}
}

int crc32_memory(uint32_t addr, uint32_t len, uint32_t* result)
{
	*result = crc32(0, (void*)addr, len);
	return 1;
}

int read_efuse(void)
{
	for(int i = 0; i < 8; i++)
	{
		WRITE_REG32(SOC_EFUSE_BASE, 1 | (i << 4));
		delay_loops(10000);
		while(1) 
			if((READ_REG32(SOC_EFUSE_BASE + 0x04) & 1) == 0)
				break;
		WRITE_REG32(cmd_buf + i * 4, READ_REG32(SOC_EFUSE_BASE + 0x08));
	}
	return 32;
}

int read_otp(void)
{
	uint32_t otp_block_size = 0;
	uint32_t otp_block_count = 0;
	uint32_t otp_interval = 0;
	uint32_t otp_start_addr = 0;
	uint32_t otp_mode = 0;
	switch(flash_id)
	{
		case 0x1440c8:
		case 0x1540c8:
			otp_interval = 0x1000;
			otp_block_size = 1024;
			otp_block_count = 2;
			break;
		case 0x1640c8:
		case 0x1840c8:
		case 0x15400b:
		case 0x16400b:
		case 0x17400b:
		case 0x17405e:
		case 0x18405e:
			otp_start_addr = 0x1000;
			otp_interval = 0x1000;
			otp_block_size = 1024;
			otp_block_count = 3;
			break;
		case 0x1460c8:
			otp_start_addr = 0x1000;
			otp_interval = 0x1000;
			otp_block_size = 512;
			otp_block_count = 3;
			break;
		case 0x164068:
		case 0x1640ef:
		case 0x144020:
		case 0x154020:
		case 0x164020:
		case 0x174020:
		case 0x14605e:
		case 0x15605e:
		case 0x16405e:
			otp_start_addr = 0x1000;
			otp_interval = 0x1000;
			otp_block_size = 256;
			otp_block_count = 3;
			break;
		case 0x15701c:
			otp_start_addr = 0x1FD000;
			otp_interval = 0x1000;
			otp_block_size = 512;
			otp_block_count = 3;
			otp_mode = 1;
			break;
		case 0x16701c:
			otp_start_addr = 0x3FD000;
			otp_interval = 0x1000;
			otp_block_size = 512;
			otp_block_count = 3;
			otp_mode = 1;
			break;
		case 0x16301c:
			otp_start_addr = 0x3FF000;
			otp_interval = 0x1000;
			otp_block_size = 512;
			otp_block_count = 1;
			otp_mode = 1;
			break;
		case 0x14400b:
			otp_start_addr = 0x1000;
			// fall through
		case 0x1540a1:
		case 0x1560eb:
		case 0x1560c4:
			otp_interval = 256;
			otp_block_size = 256;
			otp_block_count = 4;
			break;
		case 0x1660c4:
			otp_interval = 1024;
			otp_block_size = 1024;
			otp_block_count = 3;
			break;
		case 0x1720c2:
		case 0x1820c2:
			otp_interval = 512;
			otp_block_size = 512;
			otp_block_count = 2;
			break;
		case 0x146085:
			otp_start_addr = 0x1000;
			otp_interval = 0x1000;
			otp_block_size = 512;
			otp_block_count = 3;
			break;
		case 0x156085:
		case 0x154285:
		case 0x152085:
		case 0x166085:
			otp_start_addr = 0x1000;
			otp_interval = 0x1000;
			otp_block_size = 1024;
			otp_block_count = 3;
			break;
		default:
			return 0;
	}

	if(flash_id == 0x16701c || flash_id == 0x15701c || flash_id == 0x16301c)
	{
		spi_cmd_none(SPIFLASH_CMD_OTP_ENTRY);
	}

	if(flash_id == 0x1720c2 || flash_id == 0x1820c2)
	{
		spi_cmd_none(SPIFLASH_CMD_OTP_ENTRY_KH);
	}

	size_t out_offset = 0;
	for(uint32_t blk = 0; blk < otp_block_count; ++blk)
	{
		uint32_t addr = otp_start_addr + blk * otp_interval;
		uint32_t remain = otp_block_size;

		while(remain > 0)
		{
			uint32_t len = remain > 256 ? 256 : remain;
			if(otp_mode)
				spi_otp_read_region(0x48, addr, len, cmd_buf + out_offset);
			else
				spi_otp_read_region(SPIFLASH_CMD_OTP_RD, addr, len, cmd_buf + out_offset);
			addr += len;
			out_offset += len;
			remain -= len;
		}
	}

	if(flash_id == 0x16701c || flash_id == 0x15701c || flash_id == 0x16301c)
	{
		spi_cmd_none(SPIFLASH_CMD_OTP_EXIT);
	}

	if(flash_id == 0x1720c2 || flash_id == 0x1820c2)
	{
		spi_cmd_none(SPIFLASH_CMD_OTP_EXIT_KH);
	}

	return out_offset;
}

void get_chip_data(void)
{
	WRITE_REG32(cmd_buf, 0x396440B3);
}

extern int main(void);

int boot_main(void)
{
	uart_putc(0); // simulate bootrom last packet ack
	WRITE_REG32(CLK_EN_BASE, 0xFFFFFFFF);
	WRITE_REG32(CLK_DBB, (READ_REG32(CLK_DBB) & (~(1 << 2))) | (1 << 2));
	WRITE_REG32(CLK_MUX_BASE, 0x11);
	__builtin_nds32_setgie_dis();
	__nds32__enable_unaligned();
	_nds32_enable_dcache(1);
	extern uint8_t __bss_start__[];
	extern uint8_t __bss_end__[];
	memset((void*)__bss_start__, 0, (__bss_end__ - __bss_start__));
	main();
}
