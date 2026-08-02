#include "../hal_generic.h"

#define CRC_CHUNK_SIZE   (sizeof(cmd_buf) / 2)
#define CRC_CHUNK_WORDS  (CRC_CHUNK_SIZE / sizeof(uint32_t))
__attribute__((section(".dram2")))
static uint32_t crc_buf[CRC_CHUNK_WORDS] __attribute__((aligned(4)));

static volatile RDA_UART_TypeDef* UART = (volatile RDA_UART_TypeDef*)UART_BASE;
static int ChipHwVersion = 0;
uint32_t SystemCoreClock = RDA_SYS_CLK_FREQUENCY_160M; /*!< System Clock Frequency (Core Clock)*/
uint32_t AHBBusClock = RDA_BUS_CLK_FREQUENCY_80M; /*!< AHB Bus Clock Frequency (Bus Clock)*/

int rda_ccfg_hwver(void)
{
	if(0 == ChipHwVersion)
	{
		ChipHwVersion = (int)((RDA_GPIO->REVID >> 16) & 0xFFUL) + 1;
	}
	return ChipHwVersion;
}

int8_t uart_getc_timeout(uint8_t* out, uint32_t loops)
{
	while(loops--)
	{
		if(UART->LSR & RXFIFO_EMPTY_MASK)
		{
			*out = UART->RBR & 0xFF;
			return 1;
		}
	}
	return 0;
}

void uart_set_baud(uint32_t baud)
{
	uint32_t baud_divisor;
	uint32_t baud_mod;
	uint32_t ier_temp;

	baud_divisor = (AHBBusClock / baud) >> 4;
	baud_mod = (AHBBusClock / baud) & 0x0F;

	ier_temp = UART->IER;
	UART->LCR |= (1 << 7); //enable load devisor register
	UART->DLL = (baud_divisor >> 0) & 0xFF;
	UART->DLH = (baud_divisor >> 8) & 0xFF;
	UART->LCR &= ~(1 << 7);// after loading, disable load devisor register
	UART->IER = ier_temp;
	UART->LCR |= (1 << 7); //enable load devisor register
	UART->DL2 = (baud_mod >> 1) + ((baud_mod - (baud_mod >> 1)) << 4);
	UART->LCR &= ~(1 << 7);// after loading, disable load devisor register
}

void uart_init(void)
{
	uart_set_baud(115200U);
}

void rda5981_spi_flash_erase_64KB_block(uint32_t addr)
{
	spi_wip_reset();
	spi_write_reset();
	WRITE_REG32(FLASH_CTL_TX_CMD_ADDR_REG, CMD_64KB_BLOCK_ERASE | (addr << 8));
	wait_busy_down();
	spi_wip_reset();
}

static inline void wr_rf_usb_reg(unsigned char a, unsigned short d, int isusb)
{
	while(RF_SPI_REG & (0x01UL << 31));
	while(RF_SPI_REG & (0x01UL << 31));
	RF_SPI_REG = (unsigned int)d | ((unsigned int)a << 16) | (0x01UL << 25) | ((isusb) ? (0x01UL << 27) : 0x00UL);
}

static inline void rd_rf_usb_reg(unsigned char a, unsigned short* d, int isusb)
{
	while(RF_SPI_REG & (0x01UL << 31));
	while(RF_SPI_REG & (0x01UL << 31));
	RF_SPI_REG = ((unsigned int)a << 16) | (0x01UL << 24) | (0x01UL << 25) | ((isusb) ? (0x01UL << 27) : 0x00UL);
	__asm volatile ("nop");
	while(RF_SPI_REG & (0x01UL << 31));
	while(RF_SPI_REG & (0x01UL << 31));
	*d = (unsigned short)(RF_SPI_REG & 0xFFFFUL);
}
void rda_ccfg_ck(void)
{
	unsigned short val = 0U, cfg = 0U;
#if ((SYS_CPU_CLK == CLK_FREQ_160M) && (AHB_BUS_CLK == CLK_FREQ_80M))
#if 0
	const unsigned int trap_ram_code[] = {
		/* addr_ofst, val */
		/* 0x0000 */  0xB8A6F0FEUL,
		/* 0x0004 */  0xBF006008UL,
		/* 0x0008 */  0xBF00BF00UL,
		/* 0x000C */  0xBF00BF00UL,
		/* 0x0010 */  0xBF00BF00UL,
		/* 0x0014 */  0x6848BF00UL,
		/* 0x0018 */  0xD1FC07C0UL,
		/* 0x001C */  0xF000F8DFUL,
		/* 0x0020 */  0x00001EBCUL
	};
#endif
#endif /* CLK_FREQ_160M && CLK_FREQ_80M */

	cfg = (RDA_SCU->CORECFG >> 11) & 0x07U;
	rd_rf_usb_reg(0xA4, &val, 0);
#if ((SYS_CPU_CLK == CLK_FREQ_160M) && (AHB_BUS_CLK == CLK_FREQ_80M))
	/* HCLK inv */
	if(((CLK_FREQ_40M << 1) | CLK_FREQ_40M) == cfg)
	{
		val |= (0x01U << 12);
	}
#endif /* CLK_FREQ_160M && CLK_FREQ_80M */
	/* Config CPU & BUS clock */
	cfg ^= (((SYS_CPU_CLK << 1) | AHB_BUS_CLK) & 0x07U);
	val &= ~(0x07U << 9);   /* bit[11:10] = 2'b00:40M, 2'b01:80M, 2'b1x:160M */
	val |= (cfg << 9);   /* bit[9] = 1'b0:40M, 1'b1:80M */
	val &= ~(0x01U);        /* i2c_wakeup_en */
	wr_rf_usb_reg(0xA4, val, 0);

#if ((SYS_CPU_CLK == CLK_FREQ_160M) && (AHB_BUS_CLK == CLK_FREQ_80M))
#if 0
	/* Load RAM code */
	for(val = 0; val < sizeof(trap_ram_code); val += 4)
	{
		ADDR2REG(TRAP_RAM_BASE + val) = trap_ram_code[val >> 2];
	}
#endif
#endif /* CLK_FREQ_160M && CLK_FREQ_80M */
}

void flash_program_page(uint32_t off, const uint8_t* data)
{
	RDA5991H_WRITE_FLASH(off, (uint8_t*)data, FLASH_PAGE_SIZE);
}

int flash_erase_range(uint32_t src, uint32_t len)
{
	uint32_t a4k, a64k, a64kend, a4kend, atmp;
	if(len > 0x00)
	{
		a4k = ((uint32_t)src) & (~((0x01UL << 12) - 0x01UL));
		a64k = ((uint32_t)src + (0x01UL << 16) - 0x01UL) & (~((0x01UL << 16) - 0x01UL));
		a64kend = ((uint32_t)src + len) & (~((0x01UL << 16) - 0x01UL));
		a4kend = ((uint32_t)src + len + (0x01UL << 12) - 0x01UL) & (~((0x01UL << 12) - 0x01UL));

		for(atmp = a4k; atmp < a4kend; atmp += (0x01UL << 12))
		{
			if(a64kend > a64k)
			{
				if(atmp == a64k)
				{
					for(; atmp < a64kend; atmp += (0x01UL << 16))
					{
						rda5981_spi_flash_erase_64KB_block(atmp);
					}
					if(atmp == a4kend)
						break;
				}
			}
			rda5981_spi_flash_erase_4KB_sector(atmp);
		}
	}
	return 1;
}

int flash_erase_chip()
{
	memcpy(cmd_buf, (void*)FLASH_BASE, FLASH_SECTOR_SIZE);
	spi_wip_reset();
	spi_write_reset();
	WRITE_REG32(FLASH_CTL_TX_CMD_ADDR_REG, CMD_CHIP_ERASE);
	wait_busy_down();
	spi_wip_reset();
	// Write partitions back, otherwise it would become unbootable unless flashed in ROM mode.
	// Find ROM function address to write them ourselves?
	RDA5991H_WRITE_FLASH(FLASH_BASE, (uint8_t*)cmd_buf, FLASH_SECTOR_SIZE);
	return 1;
}

void flash_init(void)
{
	WRITE_REG32(FLASH_CTL_TX_BLOCK_SIZE_REG, 3 << 8);
	uint32_t status3 = ADDR2REG(FLASH_CTL_RX_FIFO_DATA_REG);
	uint32_t status4 = ADDR2REG(FLASH_CTL_RX_FIFO_DATA_REG);
	uint32_t status5 = ADDR2REG(FLASH_CTL_RX_FIFO_DATA_REG);
	wait_busy_down();
	spi_wip_reset();
	WRITE_REG32(FLASH_CTL_TX_CMD_ADDR_REG, 0x9F);
	wait_busy_down();
	status3 = ADDR2REG(FLASH_CTL_RX_FIFO_DATA_REG);
	status4 = ADDR2REG(FLASH_CTL_RX_FIFO_DATA_REG);
	status5 = ADDR2REG(FLASH_CTL_RX_FIFO_DATA_REG);
	flash_id = ((status5 & 0xFF) << 16) | ((status4 & 0xFF) << 8) | (status3 & 0xFF);
}

static void rda_crc_start(uint32_t src, uint32_t len)
{
	RDA_DMACFG->dma_src = src;
	RDA_DMACFG->dma_len = len;
	RDA_DMACFG->dma_int_out &= ~1u;
	RDA_DMACFG->dma_ctrl = 0x20000008;
}

static void dma_wait(void)
{
	while((RDA_DMACFG->dma_int_out & 1u) == 0);
	RDA_DMACFG->dma_int_out &= ~1u;
}

int crc32_memory_hardware(uint32_t addr, uint32_t len, uint32_t* result)
{
	uint8_t* src = (uint8_t*)addr;

	uint32_t chunks = len / CRC_CHUNK_SIZE;
	uint32_t tail = len % CRC_CHUNK_SIZE;

	uint32_t* buf[2] = { (uint32_t*)cmd_buf, crc_buf };

	uint32_t cur = 0;

	OR_REG32(RDA_DMACFG_BASE * 0x40000u, 0x40000);

	RDA_DMACFG->dma_func_ctrl = 0x000F0003;
	RDA_DMACFG->crc_gen = 0x04C11DB7;
	RDA_DMACFG->crc_init_val = 0xFFFFFFFF;
	RDA_DMACFG->crc_out_xorval = 0xFFFFFFFF;

	while(chunks--)
	{
		memcpy(buf[cur], src, CRC_CHUNK_SIZE);
		rda_crc_start((uint32_t)buf[cur], CRC_CHUNK_WORDS);
		dma_wait();
		src += CRC_CHUNK_SIZE;
		cur ^= 1;
	}

	if(tail)
	{
		memcpy(buf[cur ^ 1], src, tail);
		rda_crc_start((uint32_t)buf[cur ^ 1], tail >> 2);
		dma_wait();
	}

	if(result) *result = RDA_DMACFG->crc_out_val;
	return 1;
}

int read_factory_mac(uint8_t mac[6])
{
	return 0;
}

uint16_t efuse_read_page(uint8_t page)
{
	volatile uint32_t* reg = (volatile uint32_t*)0x4001301C;

	while(*reg & BIT31);

	*reg = 0x03B80000;
	for(uint32_t i = 0; i < 0x2000U; ++i) { __asm volatile("nop"); };

	while(*reg & BIT31);

	uint32_t base = (*reg & 0x0000FF00) | ((page << 4) & 0xFF);

	*reg = 0x02B80000 | base | 0x0E;
	for(uint32_t i = 0; i < 0x2000U; ++i) { __asm volatile("nop"); };

	while(*reg & BIT31);

	*reg = 0x02B80000 | base | 0x0F;
	for(uint32_t i = 0; i < 0x2000U; ++i) { __asm volatile("nop"); };

	while(*reg & BIT31);

	*reg = 0x02B80000 | base | 0x0E;
	for(uint32_t i = 0; i < 0x2000U; ++i) { __asm volatile("nop"); };

	while(*reg & BIT31);

	*reg = 0x02B80000 | base;
	for(uint32_t i = 0; i < 0x2000U; ++i) { __asm volatile("nop"); };

	while(*reg & BIT31);

	*reg = 0x03BF0000;
	for(uint32_t i = 0; i < 0x2000U; ++i) { __asm volatile("nop"); };

	while(*reg & BIT31);

	return (uint16_t)(*reg);
}

int read_efuse(void)
{
	for(uint8_t page = 0; page < 16; page++)
	{
		uint16_t value = efuse_read_page(page);

		cmd_buf[(page * 2 + 0)] = (uint8_t)(value & 0xFF);
		cmd_buf[(page * 2 + 1)] = (uint8_t)(value >> 8);
	}
	return 32;
}

extern int main(void);

__attribute__((section(".Reset_Handler"), used, naked))
void Reset_Handler(void)
{
	__asm volatile("cpsid i");
#ifdef __ARM_FP
	OR_REG32(0xE000ED88, 0xF << 20);
#endif
	extern uint8_t __bss_start__[];
	extern uint8_t __bss_end__[];
	extern uint8_t __StackTop[];
	__asm volatile ("msr msp, %0\n" :: "r" (__StackTop));
	memset((void*)__bss_start__, 0, (__bss_end__ - __bss_start__));
	void rda_ccfg_ck(void);
	rda_ccfg_ck();
	crc32_memory_hardware(0, 4, NULL); // dry run
	main();
}