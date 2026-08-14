#include "../hal_generic.h"

#define CRC_CHUNK_SIZE   (sizeof(cmd_buf) / 2)
#define CRC_CHUNK_WORDS  (CRC_CHUNK_SIZE / sizeof(uint32_t))

uint32_t SystemCoreClock = RDA_SYS_CLK_FREQUENCY_160M;
uint32_t AHBBusClock = RDA_BUS_CLK_FREQUENCY_80M;

static void wait_busy_down(void)
{
	while((READ_REG32(FLASH_CTL_STATUS_REG) & 1U) != 0U);
}

static void spi_wip_reset(void)
{
	do
	{
		WRITE_REG32(FLASH_CTL_TX_CMD_ADDR_REG, CMD_READ_STATUS_REGISTER);
		wait_busy_down();
	} while((READ_REG32(FLASH_CTL_RX_FIFO_DATA_REG) & 1U) != 0U);
}

static void spi_write_reset(void)
{
	WRITE_REG32(FLASH_CTL_TX_CMD_ADDR_REG, CMD_WRITE_ENABLE);
	wait_busy_down();
	do
	{
		WRITE_REG32(FLASH_CTL_TX_CMD_ADDR_REG, CMD_READ_STATUS_REGISTER);
		wait_busy_down();
	} while((READ_REG32(FLASH_CTL_RX_FIFO_DATA_REG) & 1U) != 0U);
}

static void flash_erase_4KB(uint32_t addr)
{
	spi_wip_reset();
	spi_write_reset();
	WRITE_REG32(FLASH_CTL_TX_CMD_ADDR_REG, CMD_4KB_SECTOR_ERASE | (addr << 8));
	wait_busy_down();
	spi_wip_reset();
}

int8_t uart_getc_timeout(uint8_t* out, uint32_t loops)
{
	while(loops--)
	{
		if(RDA_UART0->LSR & RXFIFO_EMPTY_MASK)
		{
			*out = RDA_UART0->RBR & 0xFF;
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

	ier_temp = RDA_UART0->IER;
	RDA_UART0->LCR |= (1 << 7); //enable load devisor register
	RDA_UART0->DLL = (baud_divisor >> 0) & 0xFF;
	RDA_UART0->DLH = (baud_divisor >> 8) & 0xFF;
	RDA_UART0->LCR &= ~(1 << 7);// after loading, disable load devisor register
	RDA_UART0->IER = ier_temp;
	RDA_UART0->LCR |= (1 << 7); //enable load devisor register
	RDA_UART0->DL2 = (baud_mod >> 1) + ((baud_mod - (baud_mod >> 1)) << 4);
	RDA_UART0->LCR &= ~(1 << 7);// after loading, disable load devisor register
}

void flash_erase_64KB(uint32_t addr)
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
	cfg = (RDA_SCU->CORECFG >> 11) & 0x07U;
	rd_rf_usb_reg(0xA4, &val, 0);
	if(((CLK_FREQ_40M << 1) | CLK_FREQ_40M) == cfg)
	{
		val |= (0x01U << 12);
	}
	cfg ^= (((SYS_CPU_CLK << 1) | AHB_BUS_CLK) & 0x07U);
	val &= ~(0x07U << 9);
	val |= (cfg << 9);
	val &= ~(0x01U);
	wr_rf_usb_reg(0xA4, val, 0);
}

static inline void spi_flash_flush_cache(void)
{
	WRITE_REG32(0x40014004U, 1U);

	delay_loops(8);

	while(READ_REG32(0x40014004U) & 0x80000000U);
}

static void flash_write(uint32_t addr, const void* src, uint32_t len)
{
	const uint8_t* p = src;

	while(len >= 256)
	{
		spi_wip_reset();
		spi_write_reset();

		for(unsigned i = 0; i < 256; i++) WRITE_REG8(FLASH_CTL_TX_FIFO_DATA_REG, *p++);

		WRITE_REG32(FLASH_CTL_TX_BLOCK_SIZE_REG, 256 << 8);
		WRITE_REG32(FLASH_CTL_TX_CMD_ADDR_REG, CMD_PAGE_PROGRAM | (addr << 8));

		wait_busy_down();
		spi_wip_reset();

		addr += 256;
		len -= 256;
	}

	if(len)
	{
		spi_wip_reset();
		spi_write_reset();

		for(uint32_t i = 0; i < len; i++) WRITE_REG8(FLASH_CTL_TX_FIFO_DATA_REG, *p++);

		WRITE_REG32(FLASH_CTL_TX_BLOCK_SIZE_REG, len << 8);
		WRITE_REG32(FLASH_CTL_TX_CMD_ADDR_REG, CMD_PAGE_PROGRAM | (addr << 8));

		wait_busy_down();
		spi_wip_reset();
	}
}

void flash_program_page(uint32_t off, const uint8_t* data)
{
	flash_write(off, data, FLASH_PAGE_SIZE);
	spi_flash_flush_cache();
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
						flash_erase_64KB(atmp);
					}
					if(atmp == a4kend)
						break;
				}
			}
			flash_erase_4KB(atmp);
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
	flash_write(FLASH_BASE, (uint8_t*)cmd_buf, FLASH_SECTOR_SIZE);
	return 1;
}

void flash_init(void)
{
	spi_wip_reset();
	WRITE_REG32(FLASH_CTL_TX_BLOCK_SIZE_REG, 3 << 8);
	wait_busy_down();
	spi_wip_reset();
	WRITE_REG32(FLASH_CTL_TX_CMD_ADDR_REG, CMD_READ_FLASH_ID);
	wait_busy_down();
	uint32_t status3 = READ_REG32(FLASH_CTL_RX_FIFO_DATA_REG);
	uint32_t status4 = READ_REG32(FLASH_CTL_RX_FIFO_DATA_REG);
	uint32_t status5 = READ_REG32(FLASH_CTL_RX_FIFO_DATA_REG);
	flash_id = ((status5 & 0xFF) << 16) | ((status4 & 0xFF) << 8) | (status3 & 0xFF);
}

static void rda_crc_start(uint32_t src, uint32_t len)
{
	RDA_DMACFG->dma_src = src;
	RDA_DMACFG->dma_len = len;
	RDA_DMACFG->dma_int_out &= ~AHB_DMA_DONE;
	RDA_DMACFG->dma_ctrl = 0x08 | (TX_CRC_MODE << 28);
}

static void dma_wait(void)
{
	while((RDA_DMACFG->dma_int_out & AHB_DMA_DONE) == 0);
	//RDA_DMACFG->dma_int_out &= ~AHB_DMA_DONE;
}

int crc32_memory(uint32_t addr, uint32_t len, uint32_t* result)
{
	uint8_t* src = (uint8_t*)addr;

	uint32_t chunks = len / CRC_CHUNK_SIZE;
	uint32_t tail = len % CRC_CHUNK_SIZE;

	uint32_t* buf[2] = { (uint32_t*)cmd_buf, (uint32_t*)(cmd_buf + CRC_CHUNK_SIZE) };

	uint32_t cur = 0;

	RDA_DMACFG->dma_func_ctrl = 0x000F0003;
	RDA_DMACFG->crc_gen = 0x04C11DB7;
	RDA_DMACFG->crc_init_val = 0xFFFFFFFF;
	RDA_DMACFG->crc_out_xorval = 0xFFFFFFFF;

	if(chunks == 0)
		goto tail;

	memcpy(buf[0], src, CRC_CHUNK_SIZE);
	src += CRC_CHUNK_SIZE;
	rda_crc_start((uint32_t)buf[0], CRC_CHUNK_WORDS);
	cur = 1;

	while(--chunks)
	{
		memcpy(buf[cur], src, CRC_CHUNK_SIZE);
		src += CRC_CHUNK_SIZE;
		dma_wait();
		rda_crc_start((uint32_t)buf[cur], CRC_CHUNK_WORDS);

		cur ^= 1;
	}

tail:
	if(tail)
	{
		memcpy(buf[cur ^ 1], src, tail);
		dma_wait();
		rda_crc_start((uint32_t)buf[cur ^ 1], tail >> 2);
		dma_wait();
	}
	else dma_wait();

	if(result) *result = RDA_DMACFG->crc_out_val;
	return 1;
}

uint16_t efuse_read_page(uint8_t page)
{
	volatile uint32_t* reg = (volatile uint32_t*)0x4001301C;

	while(*reg & BIT31);

	*reg = 0x03B80000;
	delay_loops(2000);

	while(*reg & BIT31);

	uint32_t base = (*reg & 0x0000FF00) | ((page << 4) & 0xFF);

	*reg = 0x02B80000 | base | 0x0E;
	delay_loops(2000);

	while(*reg & BIT31);

	*reg = 0x02B80000 | base | 0x0F;
	delay_loops(2000);

	while(*reg & BIT31);

	*reg = 0x02B80000 | base | 0x0E;
	delay_loops(2000);

	while(*reg & BIT31);

	*reg = 0x02B80000 | base;
	delay_loops(2000);

	while(*reg & BIT31);

	*reg = 0x03BF0000;
	delay_loops(2000);

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

int read_otp(void)
{
	uint32_t otp_block_size = 0;
	uint32_t otp_block_count = 0;
	uint32_t otp_interval = 0;
	uint32_t otp_start_addr = 0;
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

	size_t out_offset = 0;
	for(uint32_t blk = 0; blk < otp_block_count; ++blk)
	{
		uint32_t addr = otp_start_addr + blk * otp_interval;
		uint32_t remain = otp_block_size;

		while(remain > 0)
		{
			uint32_t len = remain > 16 ? 16 : remain;
			spi_wip_reset();
			WRITE_REG32(FLASH_CTL_TX_BLOCK_SIZE_REG, len << 8);
			wait_busy_down();
			spi_wip_reset();
			WRITE_REG32(FLASH_CTL_TX_CMD_ADDR_REG, 0x48 | (addr << 8));
			wait_busy_down();
			for(uint32_t i = 0; i < len; i++)
				(cmd_buf + out_offset)[i] = READ_REG32(FLASH_CTL_RX_FIFO_DATA_REG) & 0xFF;
			spi_wip_reset();
			addr += len;
			out_offset += len;
			remain -= len;
		}
	}

	return out_offset;
}

void get_chip_data(void)
{
	WRITE_REG32(cmd_buf, 0x7272742E); // RDA5981
	WRITE_REG32(cmd_buf + 4, RDA_GPIO->REVID);
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
	// enable dma clock
	RDA_SCU->CLKGATE0 |= (1 << 18);
	main();
}