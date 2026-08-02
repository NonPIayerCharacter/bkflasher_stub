#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "../hal_generic.h"

static uint8_t crc_buf[sizeof(cmd_buf)] __attribute__((aligned(4)));

static uint32_t w800_read_flash_jedec_id(void)
{
	/* OpenW800 internal-flash driver uses command word 0x2c09F for JEDEC 0x9F,
	 * then reads the three ID bytes from the RSA scratch window at 0x40000000.
	 */
	REG32(W800_FLASH_CMD_ADDR) = 0x0002C09FU;
	REG32(W800_FLASH_CMD_START) = W800_FLASH_CMD_START_BIT;
	delay_loops(2000);
	return REG32(W800_RSA_SCRATCH_BASE) & 0x00FFFFFFU;
}

static void w800_flash_write_enable(void)
{
	REG32(W800_FLASH_CMD_ADDR) = 0x00000006U;
	REG32(W800_FLASH_CMD_START) = W800_FLASH_CMD_START_BIT;
}

static void w800_flash_erase_sector(uint32_t off)
{
	w800_flash_write_enable();
	REG32(W800_FLASH_CMD_ADDR) = 0x80000820U;
	REG32(W800_FLASH_ADDR_REG) = off & 0x01FFFFFFU;
	REG32(W800_FLASH_CMD_START) = W800_FLASH_CMD_START_BIT;
}

static void w800_flash_erase_block(uint32_t off)
{
	w800_flash_write_enable();
	REG32(W800_FLASH_CMD_ADDR) = 0x800008D8U;
	REG32(W800_FLASH_ADDR_REG) = off & 0x01FFFFFFU;
	REG32(W800_FLASH_CMD_START) = W800_FLASH_CMD_START_BIT;
}

static void w800_crc32_hardware_start(const uint8_t* source, uint32_t len, uint32_t state)
{
	REG32(HR_CRYPTO_SEC_CTRL) = 0x4U;
	REG32(HR_CRYPTO_SEC_STS) = CRYPTO_COMPLETE_STATUS;
	REG32(HR_CRYPTO_SEC_CFG) = CRYPTO_CRC32_CONFIG | len;
	REG32(HR_CRYPTO_CRC_KEY) = reverse_u32(state);
	REG32(HR_CRYPTO_SRC_ADDR) = (uint32_t)(uintptr_t)source;
	REG32(HR_CRYPTO_SEC_CTRL) = 0x1U;
}

static int w800_crc32_hardware_wait(uint32_t* state)
{
	uint32_t timeout = CRYPTO_WAIT_LOOPS;
	while(!(REG32(HR_CRYPTO_SEC_STS) & CRYPTO_COMPLETE_STATUS) && timeout) timeout--;
	if(!timeout)
	{
		REG32(HR_CRYPTO_SEC_CTRL) = 0x4U;
		return 0;
	}
	REG32(HR_CRYPTO_SEC_STS) = CRYPTO_COMPLETE_STATUS;
	*state = REG32(HR_CRYPTO_CRC_RESULT);
	REG32(HR_CRYPTO_SEC_CTRL) = 0x4U;
	return 1;
}

static int w800_read_factory_mac_at(uint32_t flash_off, uint8_t mac[6])
{
	const volatile uint8_t* param = (const volatile uint8_t*)(uintptr_t)(FLASH_BASE + flash_off);
	uint8_t bytes[FT_PARAM_SIZE];
	memcpy(bytes, (const void*)param, sizeof(bytes));
	if(load_le32(bytes) != FT_PARAM_MAGIC) return 0;

	uint32_t crc = 0xFFFFFFFFU;
	for(uint32_t i = 8U; i < sizeof(bytes); i++) crc = crc32_update_wire(crc, bytes[i]);
	if(crc != load_le32(bytes + 4U)) return 0;

	uint8_t any = 0U;
	uint8_t all_ff = 0xFFU;
	for(uint32_t i = 0U; i < 6U; i++)
	{
		mac[i] = bytes[8U + i];
		any |= mac[i];
		all_ff &= mac[i];
	}
	return (mac[0] & 1U) == 0U && any != 0U && all_ff != 0xFFU;
}

void uart_set_baud(uint32_t baud)
{
	uint32_t divisor_16ths = (APB_CLK + baud / 2U) / baud;
	uint32_t div = divisor_16ths / 16U - 1U;
	uint32_t frac = divisor_16ths % 16U;
	REG32(HR_UART0_BAUD_RATE_CTRL) = div | (frac << 16);
}

void uart_init(void)
{
	uart_set_baud(115200U);
	REG32(HR_UART0_LINE_CTRL) = ULCON_WL8 | ULCON_TX_EN | ULCON_RX_EN;
	REG32(HR_UART0_FLOW_CTRL) = 0U;
	REG32(HR_UART0_DMA_CTRL) = 0U;
	REG32(HR_UART0_FIFO_CTRL) = 0U;
	REG32(HR_UART0_INT_MASK) = 0xFFFFFFFFU;
}

static int uart0_rx_count(void)
{
	return (int)((REG32(HR_UART0_FIFO_STATUS) & UFS_RX_FIFO_CNT_MASK) >> UFS_RX_FIFO_CNT_SHIFT);
}

void uart_putc(uint8_t b)
{
	/* Conservative: wait for TX FIFO to drain. This is slow but robust for early stub work. */
	while(REG32(HR_UART0_FIFO_STATUS) & UFS_TX_FIFO_CNT_MASK) {}
	REG32(HR_UART0_TX_WIN) = (uint32_t)b;
}

int8_t uart_getc_timeout(uint8_t* out, uint32_t loops)
{
	while(loops--)
	{
		if(uart0_rx_count() > 0)
		{
			*out = (uint8_t)(REG32(HR_UART0_RX_WIN) & 0xFFU);
			return 1;
		}
	}
	return 0;
}

void flash_program_page(uint32_t off, const uint8_t* data)
{
	const alias_u32* words = (const alias_u32*)(const void*)data;
	for(uint32_t i = 0U; i < FLASH_PAGE_SIZE / 4U; i++)
	{
		REG32(W800_RSA_SCRATCH_BASE + i * 4U) = words[i];
	}
	w800_flash_write_enable();
	REG32(W800_FLASH_CMD_ADDR) = 0x80FF9002U;
	REG32(W800_FLASH_ADDR_REG) = off & 0x01FFFFFFU;
	REG32(W800_FLASH_CMD_START) = W800_FLASH_CMD_START_BIT;
}

int flash_erase_range(uint32_t off, uint32_t len)
{
	uint32_t end = off + len;
	while(off < end)
	{
		uint32_t erase_size = FLASH_SECTOR_SIZE;
		if((off & (FLASH_BLOCK_SIZE - 1U)) == 0U && end - off >= FLASH_BLOCK_SIZE)
		{
			w800_flash_erase_block(off);
			erase_size = FLASH_BLOCK_SIZE;
		}
		else
		{
			w800_flash_erase_sector(off);
		}
		//if(!flash_range_is_erased(off, erase_size)) return 0;
		off += erase_size;
	}
	return 1;
}

int flash_erase_chip()
{
	return flash_erase_range(FLASH_WRITE_MIN_OFFSET, flash_size - FLASH_WRITE_MIN_OFFSET);
}

void flash_init(void)
{
	flash_id = w800_read_flash_jedec_id();
}

int crc32_memory_hardware(uint32_t addr, uint32_t len, uint32_t* result)
{
	uint32_t state = 0xFFFFFFFFU;
	uint8_t* current = cmd_buf;
	uint8_t* next = crc_buf;
	uint32_t chunk = len > sizeof(cmd_buf) ? sizeof(cmd_buf) : len;

	/* The crypto DMA cannot consume the CPU QFLASH mapping directly. */
	memcpy(current, (const void*)(uintptr_t)addr, chunk);
	while(chunk)
	{
		w800_crc32_hardware_start(current, chunk, state);
		addr += chunk;
		len -= chunk;

		uint32_t next_chunk = len > sizeof(cmd_buf) ? sizeof(cmd_buf) : len;
		if(next_chunk) memcpy(next, (const void*)(uintptr_t)addr, next_chunk);
		if(!w800_crc32_hardware_wait(&state)) return 0;

		uint8_t* swap = current;
		current = next;
		next = swap;
		chunk = next_chunk;
	}
	*result = state;
	return 1;
}

int read_factory_mac(uint8_t mac[6])
{
	if(w800_read_factory_mac_at(FT_PARAM_RUNTIME_OFFSET, mac)) return 1;
	return w800_read_factory_mac_at(0U, mac);
}

int read_efuse(void)
{
	return 0;
}