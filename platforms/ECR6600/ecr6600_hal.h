#ifndef ECR6600_HAL_H
#define ECR6600_HAL_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#undef BUF_SIZE
#define BUF_SIZE 1024

#define SOC_PD_SMU_BASE					(0x00202000)
#define SOC_RFC_BASE					(0x00203000)
#define SOC_UART0_BASE					(0x00204000)
#define SOC_SPI0_BASE					(0x0020B000)
#define FLASH_BASE						(0x00800000)
#define UARTC_OSCR_REG					(SOC_UART0_BASE + 0x14)
#define UARTC_DLL_REG					(SOC_UART0_BASE + 0x20)
#define UARTC_THR_REG					(SOC_UART0_BASE + 0x20)
#define UARTC_DLM_REG					(SOC_UART0_BASE + 0x24)
#define UARTC_LCR_REG					(SOC_UART0_BASE + 0x2c)
#define UARTC_LSR_REG					(SOC_UART0_BASE + 0x34)
#define UARTC_LSR_THRE					(0x20)
#define UARTC_LSR_TEMT					(0x40)
#define EFUSE_CTRL_OFFSET				(0x20)
#define EFUSE_CRYSTAL_26M				(0x0 << 16)
#define EFUSE_CRYSTAL_40M				(0x1 << 16)
#define EFUSE_CRYSTAL_OFFSET			(0x1 << 16)
#define SOC_PD_CLK_MUX_BASE				(SOC_PD_SMU_BASE + 0x4)
#define SOC_PD_CLK_EN_BASE0				(SOC_PD_SMU_BASE + 0xC)
#define SOC_PD_SPI_CONFIG				(SOC_PD_SMU_BASE + 0x18)
#define SOC_PD_PIN_MUX0					(SOC_PD_SMU_BASE + 0x30)
#define SOC_PD_PIN_MUX1					(SOC_PD_SMU_BASE + 0x34)
#define SOC_PD_UART_CLK_SEL				(SOC_PD_SMU_BASE + 0x50)
#define SOC_PD_APB_ENCRYPT_EN			(SOC_PD_SMU_BASE + 0x78)
#define UART_CLK_SEL_40M				(0x0)
#define UART_CLK_SEL_160M_DIV2			(0x1)
#define CLK_MUX_EXT_CLK_40M				(0x1 << 4)
#define CLK_MUX_CRYSTAL_40M				(0x0 << 4)
#define CPU_CLK_FREQ_DIV2				(0x0 << 2)
#define CPU_CLK_FREQ_DIV0				(0x1 << 2)
#define CPU_CLK_FREQ_DIV4				(0x2 << 2)
#define CPU_CLK_FREQ_DIV8				(0x3 << 2)
#define CPU_CLK_SEC_40M_26M				(0x0 << 0)
#define CPU_CLK_SEC_240M				(0x1 << 0)
#define CPU_CLK_SEC_160M				(0x2 << 0)
#define CPU_CLK_SEC_32K					(0x3 << 0)
#define SOC_RFC_DIG_PLL1				(SOC_RFC_BASE + 0x84)
#define SOC_RFC_DIG_PLL2				(SOC_RFC_BASE + 0x88)
#define SOC_RFC_DIG_PLL3				(SOC_RFC_BASE + 0x8C)
#define SOC_DIG_PLL_REG12				(SOC_RFC_BASE + 0x130)
#define SOC_DIG_PLL_REG13				(SOC_RFC_BASE + 0x134)
#define SOC_DIG_PLL_REG14				(SOC_RFC_BASE + 0x138)
#define SOC_DIG_PLL_REG18				(SOC_RFC_BASE + 0x148)
#define SOC_DIG_PLL_REG20				(SOC_RFC_BASE + 0x150)
#define FREF_CLK_EN_BIT					(1<<10)
#define RF_REGF_APB_CLK_BIT				(1<<19)
#define SPI0_APB_CLK_BIT				(1<<7)
#define FLASH_AHB_CLK_BIT				(1<<28)
#define APB_WRIT_ENCRYPT				(0x1)
#define APB_WRIT_NOT_ENCRYPT			(0x0)
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
#define SPIFLASH_CMD_OTP_RD				0x48
#define SPIFLASH_CMD_OTP_ENTRY			0x3A
#define SPIFLASH_CMD_OTP_EXIT			0x04
#define SPIFLASH_CMD_OTP_ENTRY_KH		0xB1
#define SPIFLASH_CMD_OTP_EXIT_KH		0xC1
#define SPIFLASH_STATUS_WIP				0x01
#define SPIFLASH_STATUS_WEL				0x02
#define SPIFLASH_STATUS_BPX				0x7C
#define SPIFLASH_STATUS_QE				0x0200
#define SPIFLASH_STATUS_2_QE			0x02
#define SPIFLASH_STATUS_RSTEN			0x66
#define SPIFLASH_STATUS_RST				0x99
#define SPI_STATUS_RXENPTY				BIT(14)

#define PIN_MUX_SET(reg,bits,offset,fun) WRITE_REG32(reg, (READ_REG32(reg) & (~(bits<<offset))) | (fun<<offset))
#define PIN_FUNC_SET(PIN_NAME,PIN_FUNC) PIN_MUX_SET(PIN_NAME##_REG, PIN_NAME##_BITS, PIN_NAME##_OFFSET, PIN_FUNC)
#define IO_MUX0_GPIO7_REG				SOC_PD_PIN_MUX0
#define IO_MUX0_GPIO7_BITS				7 
#define IO_MUX0_GPIO7_OFFSET			28
#define IO_MUX1_GPIO8_REG				SOC_PD_PIN_MUX1
#define IO_MUX1_GPIO8_BITS				7 
#define IO_MUX1_GPIO8_OFFSET			0
#define IO_MUX1_GPIO9_REG				SOC_PD_PIN_MUX1
#define IO_MUX1_GPIO9_BITS				7 
#define IO_MUX1_GPIO9_OFFSET			4
#define IO_MUX1_GPIO10_REG				SOC_PD_PIN_MUX1
#define IO_MUX1_GPIO10_BITS				7 
#define IO_MUX1_GPIO10_OFFSET			8
#define IO_MUX1_GPIO11_REG				SOC_PD_PIN_MUX1
#define IO_MUX1_GPIO11_BITS				7 
#define IO_MUX1_GPIO11_OFFSET			12
#define IO_MUX1_GPIO12_REG				SOC_PD_PIN_MUX1
#define IO_MUX1_GPIO12_BITS				7 
#define IO_MUX1_GPIO12_OFFSET			16
#define FUNC_GPIO7_MSPI_MOSI			2
#define FUNC_GPIO8_MSPI_HOLD			2
#define FUNC_GPIO9_MSPI_CLK 			2
#define FUNC_GPIO10_MSPI_CS0			2
#define FUNC_GPIO11_MSPI_MISO			2
#define FUNC_GPIO12_MSPI_WP 			2

typedef enum
{
	SPI_MODE_STANDARD = 0,
	SPI_MODE_DUAL,
	SPI_MODE_QUAD,
	SPI_MODE_QPI,
	SPI_MODE_UNKNOW
} E_SPI_MODE;

typedef enum
{
	SPIFLASH_READ_CMD_03 = 0,
	SPIFLASH_READ_CMD_0B,
	SPIFLASH_READ_CMD_3B,
	SPIFLASH_READ_CMD_6B,
	SPIFLASH_READ_CMD_BB,
	SPIFLASH_READ_CMD_EB,
	SPIFLASH_READ_CMD_MAX
} E_SPIFLASH_READ_CMD;

int uart_data_tstc(unsigned int uart_base);
//void uart_init_ecr(unsigned int uart_base);
int uart_getc(unsigned int uart_base, unsigned char* s, unsigned int timeout);
unsigned int crc32(unsigned int uCRC32Value, const void* pbuf, unsigned int length);
void sha256(const unsigned char* message, unsigned int len, unsigned char* digest);
unsigned int efuse_read(unsigned int addr);
void efuse_read_series(unsigned int addr, unsigned int* value, unsigned int length);
void efuse_cfg_160m(void);
void udelay(unsigned int delay);
int spi_bus_ready(unsigned int baseAddr);
int spi_fifo_reset(unsigned int baseAddr);
int spi_rx_ready(unsigned int baseAddr);

#endif
