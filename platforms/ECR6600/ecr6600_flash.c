#include "../hal_generic.h"

E_SPI_MODE spi_mode = SPI_MODE_STANDARD;
extern uint32_t flash_id;

static int spi_cmd_none(uint8_t cmd)
{
	if(!spi_bus_ready(SOC_SPI0_BASE)) return -1;
	if(spi_fifo_reset(SOC_SPI0_BASE)) return -2;

	if(spi_mode == SPI_MODE_QPI)
	{
		WRITE_REG32(SOC_SPI0_BASE + 0x20, SPI_TRANSCTRL_TRAMODE_WO | SPI_TRANSCTRL_DUALQUAD_QUAD);
		WRITE_REG32(SOC_SPI0_BASE + 0x2c, cmd);
	}
	else
	{
		WRITE_REG32(SOC_SPI0_BASE + 0x20, SPI_TRANSCTRL_CMD_EN | SPI_TRANSCTRL_TRAMODE_NONE);
		WRITE_REG32(SOC_SPI0_BASE + 0x24, cmd);
	}

	return 0;
}

static uint32_t spi_read_cmd(uint8_t cmd, uint32_t len)
{
	uint32_t enc = READ_REG32(SOC_PD_APB_ENCRYPT_EN);
	WRITE_REG32(SOC_PD_APB_ENCRYPT_EN, APB_WRIT_NOT_ENCRYPT);
	if(!spi_bus_ready(SOC_SPI0_BASE)) goto fail;
	if(spi_fifo_reset(SOC_SPI0_BASE)) goto fail;
	WRITE_REG32(SOC_SPI0_BASE + 0x20, SPI_TRANSCTRL_CMD_EN | SPI_TRANSCTRL_TRAMODE_RO | SPI_TRANSCTRL_RCNT(len - 1));
	WRITE_REG32(SOC_SPI0_BASE + 0x24, cmd);
	if(!spi_rx_ready(SOC_SPI0_BASE))  goto fail;
	WRITE_REG32(SOC_PD_APB_ENCRYPT_EN, enc);
	return READ_REG32(SOC_SPI0_BASE + 0x2c);

fail:
	WRITE_REG32(SOC_PD_APB_ENCRYPT_EN, enc);
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
	unsigned int enc = READ_REG32(SOC_PD_APB_ENCRYPT_EN);
	WRITE_REG32(SOC_PD_APB_ENCRYPT_EN, APB_WRIT_NOT_ENCRYPT);
	flash_write_enable();
	WRITE_REG32(SOC_SPI0_BASE + 0x20, SPI_TRANSCTRL_CMD_EN | SPI_TRANSCTRL_TRAMODE_WO | SPI_TRANSCTRL_WCNT(len - 1));
	WRITE_REG32(SOC_SPI0_BASE + 0x24, cmd);
	WRITE_REG32(SOC_SPI0_BASE + 0x2c, value);
	flash_wait_busy();
	WRITE_REG32(SOC_PD_APB_ENCRYPT_EN, enc);
}

static void flash_enable_quad(void)
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
}

E_SPI_MODE flash_init_ecr(E_SPI_SCLK_DIV clk_div, E_SPIFLASH_READ_CMD xip_cmd)
{
	OR_REG32(SOC_PD_CLK_EN_BASE0, SPI0_APB_CLK_BIT | FLASH_AHB_CLK_BIT);
	WRITE_REG32(SOC_PD_SMU_BASE + 0x18, 0x80);

	PIN_FUNC_SET(IO_MUX0_GPIO7, FUNC_GPIO7_MSPI_MOSI);
	PIN_FUNC_SET(IO_MUX1_GPIO8, FUNC_GPIO8_MSPI_HOLD);
	PIN_FUNC_SET(IO_MUX1_GPIO9, FUNC_GPIO9_MSPI_CLK);
	PIN_FUNC_SET(IO_MUX1_GPIO10, FUNC_GPIO10_MSPI_CS0);
	PIN_FUNC_SET(IO_MUX1_GPIO11, FUNC_GPIO11_MSPI_MISO);
	PIN_FUNC_SET(IO_MUX1_GPIO12, FUNC_GPIO12_MSPI_WP);

	spi_cmd_none(SPIFLASH_STATUS_RSTEN);
	spi_cmd_none(SPIFLASH_STATUS_RST);
	udelay(4 * 1000);
	WRITE_REG32(SOC_SPI0_BASE + 0x40, (READ_REG32(SOC_SPI0_BASE + 0x40) & 0xffffff00) | clk_div);
	WRITE_REG32(SOC_SPI0_BASE + 0x50, (READ_REG32(SOC_SPI0_BASE + 0x50) & 0xf) | xip_cmd);

	flash_id = spi_read_cmd(SPIFLASH_CMD_RDID, 3) & 0xFFFFFF;

	if(xip_cmd == SPIFLASH_READ_CMD_EB && flash_id != 0xFFFFFF)
	{
		flash_enable_quad();
		spi_mode = SPI_MODE_QUAD;
	}
	else
	{
		spi_mode = SPI_MODE_STANDARD;
	}

	return spi_mode;
}

int flash_page_program(uint32_t addr, const uint8_t* buf, uint32_t len)
{
	flash_write_enable();
	if(!spi_bus_ready(SOC_SPI0_BASE)) return -1;
	if(spi_fifo_reset(SOC_SPI0_BASE)) return -2;

	if(spi_mode == SPI_MODE_QPI)
	{
		uint32_t cmd = ((addr & 0xff) << 24) | (((addr >> 8) & 0xff) << 16) | (((addr >> 16) & 0xff) << 8) | SPIFLASH_CMD_PP;
		WRITE_REG32(SOC_SPI0_BASE + 0x20, SPI_TRANSCTRL_TRAMODE_WO | SPI_TRANSCTRL_WCNT(len + 3));
		WRITE_REG32(SOC_SPI0_BASE + 0x24, 0);
		WRITE_REG32(SOC_SPI0_BASE + 0x2c, cmd);
	}
	else
	{
		WRITE_REG32(SOC_SPI0_BASE + 0x20, SPI_TRANSCTRL_CMD_EN | SPI_TRANSCTRL_ADDR_EN | SPI_TRANSCTRL_TRAMODE_WO | SPI_TRANSCTRL_WCNT(len - 1));
		WRITE_REG32(SOC_SPI0_BASE + 0x24, SPIFLASH_CMD_PP);
	}

	uint32_t words = (len + 3) / 4;

	for(uint32_t i = 0; i < words; i++)
	{
		while(READ_REG32(SOC_SPI0_BASE + 0x34) & SPI_STATUS_TXFULL);
		WRITE_REG32(SOC_SPI0_BASE + 0x28, addr + i * 4);
		WRITE_REG32(SOC_SPI0_BASE + 0x2c, ((uint32_t*)buf)[i]);
	}

	flash_wait_busy();

	return 0;
}

int flash_quad_page_program(uint32_t addr, const uint8_t* buf, uint32_t len)
{
	flash_write_enable();
	if(!spi_bus_ready(SOC_SPI0_BASE)) return -1;
	if(spi_fifo_reset(SOC_SPI0_BASE)) return -2;

	WRITE_REG32(SOC_SPI0_BASE + 0x28, addr);
	WRITE_REG32(SOC_SPI0_BASE + 0x20, SPI_TRANSCTRL_CMD_EN | SPI_TRANSCTRL_ADDR_EN | SPI_TRANSCTRL_TRAMODE_WO | SPI_TRANSCTRL_DUALQUAD_QUAD | SPI_TRANSCTRL_WCNT(len - 1));
	WRITE_REG32(SOC_SPI0_BASE + 0x24, SPIFLASH_CMD_QUAD_INPUT_PP);

	uint32_t words = (len + 3) / 4;

	for(uint32_t i = 0; i < words; i++)
	{
		while(READ_REG32(SOC_SPI0_BASE + 0x34) & SPI_STATUS_TXFULL);
		WRITE_REG32(SOC_SPI0_BASE + 0x28, addr + i * 4);
		WRITE_REG32(SOC_SPI0_BASE + 0x2c, ((uint32_t*)buf)[i]);
	}

	flash_wait_busy();

	return 0;
}

int flash_write(uint32_t addr, const uint8_t* buf, uint32_t len)
{
	while(len)
	{
		uint32_t page_off = addr % FLASH_PAGE_SIZE;

		uint32_t chunk = FLASH_PAGE_SIZE - page_off;

		if(chunk > len) chunk = len;

		if(spi_mode == SPI_MODE_QUAD)
		{
			if(flash_quad_page_program(addr, buf, chunk)) return -1;
		}
		else
		{
			if(flash_page_program(addr, buf, chunk)) return -1;
		}

		addr += chunk;
		buf += chunk;
		len -= chunk;
	}

	return 0;
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

int flash_erase_sector(uint32_t addr)
{
	if(addr % FLASH_SECTOR_SIZE) return -1;
	return flash_erase_cmd(addr, SPIFLASH_CMD_SE);
}

int flash_erase_block(uint32_t addr)
{
	if(addr % FLASH_BLOCK_SIZE) return -1;
	return flash_erase_cmd(addr, SPIFLASH_CMD_BE);
}

int flash_chip_erase(void)
{
	flash_write_enable();
	spi_cmd_none(SPIFLASH_CMD_CE);
	flash_wait_busy();
	return 0;
}
