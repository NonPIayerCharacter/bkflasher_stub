#ifndef TR6260_HAL_H
#define TR6260_HAL_H

#include <stdint.h>
#include <stdbool.h>
#include <nds32_intrinsic.h>
#undef BUF_SIZE
#define BUF_SIZE 0x1000

#define UART0_BASE						(0x00602000)
#define CLK_MUX_BASE					(0x00601804)
#define CLK_EN_BASE						(0x0060180C)
#define CLK_DBB							(0x00601CAC)
#define SOC_EFUSE_BASE					(0x0060B200)
#define SPI2_BASE						(0x0060A000)
#define SOC_SPI0_BASE					SPI2_BASE
#define SPI_TRANSCTRL_CMD_EN			(1<<30)
#define SPI_STATUS_TXFULL				BIT(23)
#define SPI_TRANSCTRL_RCNT(x)			(((x) & 0x1FF) << 0)
#define SPI_TRANSCTRL_WCNT(x)			(((x) & 0x1FF) << 12)
#define SPI_TRANSCTRL_DUALQUAD(x)		(((x) & 0x3) << 22)
#define SPI_TRANSCTRL_TRAMODE(x)		(((x) & 0xF) << 24)
#define SPI_TRANSCTRL_DUALQUAD_REGULAR	SPI_TRANSCTRL_DUALQUAD(0)
#define SPI_TRANSCTRL_DUALQUAD_DUAL		SPI_TRANSCTRL_DUALQUAD(1)
#define SPI_TRANSCTRL_DUALQUAD_QUAD		SPI_TRANSCTRL_DUALQUAD(2)
#define SPI_TRANSCTRL_TRAMODE_WO		SPI_TRANSCTRL_TRAMODE(1)
#define SPI_TRANSCTRL_TRAMODE_RO		SPI_TRANSCTRL_TRAMODE(2)
#define SPI_TRANSCTRL_TRAMODE_NONE		SPI_TRANSCTRL_TRAMODE(7)
#define SPI_TRANSCTRL_ADDR_EN			(1<<29)
#define SPI_TRANSCTRL_ADDR_FMT			(1<<28)
#define SPIFLASH_CMD_WREN				0x06
#define SPIFLASH_CMD_RDID				0x9F
#define SPIFLASH_CMD_RDSR_1				0x05
#define SPIFLASH_CMD_RDSR_2				0x35
#define SPIFLASH_CMD_WRSR				0x01
#define SPIFLASH_CMD_WRSR_2				0x31
#define SPIFLASH_CMD_PP					0x02
#define SPIFLASH_CMD_QUAD_INPUT_PP		0x32
#define SPIFLASH_CMD_SE					0x20
#define SPIFLASH_CMD_HE					0x52
#define SPIFLASH_CMD_BE					0xD8
#define SPIFLASH_CMD_CE					0xC7
#define SPIFLASH_STATUS_WIP				0x01
#define SPIFLASH_STATUS_WEL				0x02
#define SPIFLASH_STATUS_BPX				0x7C
#define SPIFLASH_STATUS_QE				0x0200
#define SPIFLASH_STATUS_2_QE			0x02
#define SPIFLASH_STATUS_RSTEN			0x66
#define SPIFLASH_STATUS_RST				0x99
typedef enum
{
	SPI_MODE_STANDARD = 0,
	SPI_MODE_DUAL,
	SPI_MODE_QUAD,
	SPI_MODE_QPI,
	SPI_MODE_UNKNOW
} E_SPI_MODE;

#define FLASH_BASE 0x40C00000

int uart_data_tstc(unsigned int regBase);
unsigned char uart_data_getc(unsigned int regBase);
uint32_t crc32(uint32_t crc, const void* buf, size_t size);

extern void _nds32_enable_dcache(int On);
int spi_bus_ready(unsigned int baseAddr);
int spi_fifo_reset(unsigned int baseAddr);
int spi_rx_ready(unsigned int baseAddr);
void spiFlash_init(unsigned int timing, unsigned int rdCmd);

#endif
