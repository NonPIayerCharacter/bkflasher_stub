#include "../hal_generic.h"

#define REG8(address) (*(volatile uint8_t *)(uintptr_t)(address))

static uint32_t baud_reg;
static uint32_t primary_size;
static uint32_t secondary_id;
static uint32_t secondary_state = SECONDARY_UNSUPPORTED;
static uint32_t secondary_size;
static uint32_t secondary_initial_state[4];

static void bitband_write(uint32_t address, uint32_t bit, uint32_t value)
{
	uint32_t alias = 0x42000000u + ((address - 0x40000000u) * 32u) + (bit * 4u);
	REG32(alias) = value;
}

static void secboot_gpio_output_low_floating(uint32_t bit)
{
	REG32(GPIOB_AF_SELECT) &= ~bit;
	REG32(GPIOB_DATA_ENABLE) |= bit;
	REG32(GPIOB_DIRECTION) |= bit;
	REG32(GPIOB_PULL_ENABLE) |= bit;
	REG32(GPIOB_DATA) &= ~bit;
}

static void secboot_uart_register_init(uint32_t base, uint32_t baud_register)
{
	REG32(base + 0x10u) = baud_register;
	REG32(base + 0x00u) = 0xC3u;
	REG32(base + 0x04u) = 0u;
	REG32(base + 0x08u) = 0u;
	REG32(base + 0x0Cu) = 0u;
	REG32(base + 0x14u) = 0u;
}

static void secboot_spi_reset_and_normalize_gpio(void)
{
	bitband_write(CLOCK_RESET_CONTROL, 12u, 0u);
	bitband_write(CLOCK_RESET_CONTROL, 12u, 1u);
	REG32(GPIOB_AF_SELECT) &= 0xF7FFFFF0u;
}

static void secboot_spi_open_close(void)
{
	secboot_spi_reset_and_normalize_gpio();

	REG32(GPIOA_AF_SELECT) |= SECBOOT_SPI_PINS;
	REG32(GPIOA_AF_SELECT_1) |= (1u << 11) | (1u << 9) | (1u << 3) | (1u << 2);
	REG32(GPIOA_AF_SELECT_0) |= (1u << 11) | (1u << 9);
	REG32(GPIOA_AF_SELECT_0) &= ~((1u << 3) | (1u << 2));

	REG32(SPI_CHANNEL_CONFIG) = 0x00580006u;
	REG32(SPI_CONFIG) = 4u;
	REG32(SPI_CLOCK_CONFIG) = 1u;
	REG32(SPI_INT_MASK) = 0xFFu;
	REG32(SPI_INT_STATUS) = 0xFFu;
	REG32(NVIC_CLEAR_PENDING_0) = 1u << 21;

	secboot_spi_reset_and_normalize_gpio();
	REG32(GPIOA_AF_SELECT) &= ~SECBOOT_SPI_PINS;
}

static void reproduce_secboot_pre_application_state(uint32_t baud_register)
{
	static const uint8_t gpio_bits[] = {8u, 9u, 13u, 14u, 15u, 16u, 17u, 18u};
	uint32_t i;

	for (i = 0u; i < sizeof(gpio_bits); ++i) {
		secboot_gpio_output_low_floating(1u << gpio_bits[i]);
	}
	REG32(GPIOB_PULL_ENABLE) &= ~(1u << 11);
	REG32(GPIOA_PULL_ENABLE) &= ~GPIOA_UART0_RX;

	secboot_uart_register_init(UART0_BASE, UART_BAUD_115200);
	secboot_uart_register_init(UART1_BASE, UART_BAUD_115200);
	secboot_spi_open_close();
	REG32(NVIC_CLEAR_PENDING_1) = 0x80u;

	//secboot_uart_register_init(UART0_BASE, baud_register);
}

void uart_putc(uint8_t b)
{
	while((REG32(UART0_FIFO_STATUS) & UART_TX_COUNT));
	REG8(UART0_TX_WINDOW) = b;
}

int8_t uart_getc_timeout(uint8_t* out, uint32_t loops)
{
	while(loops--)
	{
		if((REG32(UART0_FIFO_STATUS) & UART_RX_COUNT) > 0)
		{
			*out = (uint8_t)(REG32(UART0_RX_WINDOW) & 0xFFU);
			return 1;
		}
	}
	return 0;
}

void uart_set_baud(uint32_t baud)
{
	uint32_t divisor_16ths = (APB_CLK + baud / 2U) / baud;
	uint32_t div = divisor_16ths / 16U - 1U;
	uint32_t frac = divisor_16ths % 16U;
	REG32(UART0_BR_CTRL) = div | (frac << 16);
}

void uart_init()
{
	reproduce_secboot_pre_application_state(baud_reg);
	//uart_set_baud(115200);
	//delay_loops(10000);
	//uart_putc('W');
	//uart_putc('6');
	//uart_putc('0');
	//uart_putc('0');
	//uart_putc('S');
}

//void stub_flash_read(void* dest, uint32_t off, size_t len)
/* Read-only transaction matching the vendor SDK's internal QFLASH path. */
static void flash_read(uint32_t cpu_address, uint8_t *destination, uint32_t length)
{
	uint32_t completed = 0u;

	while (completed < length) {
		uint32_t block_length = length - completed;
		uint32_t offset = cpu_address + completed - FLASH_BASE;
		uint32_t words;
		uint32_t i;
		uint32_t word;
		uint32_t produced = 0u;

		if (block_length > FLASH_READ_WINDOW) {
			block_length = FLASH_READ_WINDOW;
		}
		words = (block_length + 3u) / 4u;
		REG32(FLASH_COMMAND) = FLASH_READ_COMMAND | (((block_length - 1u) & 0x3FFu) << 16);
		REG32(FLASH_ADDRESS) = 0x10000000u | ((offset & 0xFFFFFu) << 8);

		for (i = 0u; i < words; ++i) {
			word = REG32(FLASH_DATA_WINDOW + (i * 4u));
			if (produced < block_length) destination[completed + produced++] = (uint8_t)word;
			if (produced < block_length) destination[completed + produced++] = (uint8_t)(word >> 8);
			if (produced < block_length) destination[completed + produced++] = (uint8_t)(word >> 16);
			if (produced < block_length) destination[completed + produced++] = (uint8_t)(word >> 24);
		}
		completed += block_length;
	}
}

static uint32_t flash_read_jedec_id(void)
{
	REG32(FLASH_COMMAND) = FLASH_JEDEC_ID_COMMAND;
	REG32(FLASH_ADDRESS) = FLASH_TRIGGER;
	return REG32(FLASH_DATA_WINDOW) & 0x00FFFFFFu;
}

static int spi_wait(uint32_t address, uint32_t mask, uint32_t value)
{
	uint32_t polls;
	for (polls = 0u; polls < SPI_POLL_LIMIT; ++polls) {
		if ((REG32(address) & mask) == value) {
			return 1;
		}
	}
	return 0;
}

static int secondary_read(uint8_t opcode, uint32_t offset, uint8_t *data, uint32_t length)
{
	static const uint32_t registers[] = {
		GPIOA_AF_SELECT, GPIOA_AF_SELECT_1, GPIOA_AF_SELECT_0,
		GPIOB_AF_SELECT, GPIOB_AF_SELECT + 4u, GPIOB_AF_SELECT + 8u,
		SPI_CHANNEL_CONFIG, SPI_CONFIG, SPI_CLOCK_CONFIG, SPI_BASE + 0xCu,
		SPI_INT_MASK, SPI_BASE + 0x1Cu
	};
	static const uint8_t alternatives[][3] = {
		{0,1,3}, {0,4,3}, {0,5,3}, {0,10,4}, {0,12,4},
		{1,0,1}, {1,1,1}, {1,2,1}, {1,7,4}, {1,15,3},
		{1,16,3}, {1,17,3}, {1,18,3}, {1,27,1}
	};
	uint32_t saved[sizeof(registers) / sizeof(registers[0])];
	uint32_t i, j, word, count, command_length, clocks, bit, base, option;
	uint32_t byte;
	uint32_t reset_bit;
	uint32_t initial_flags;
	uint32_t pending_bit;
	int ok = 0;
	initial_flags = REG32(SPI_INT_STATUS) & 0xFFu;
	pending_bit = REG32(0xE000E200u) & (1u << 21);
	if ((REG32(SPI_STATUS) & (SPI_BUSY | SPI_FIFO_COUNTS)) != 0u ||
		(initial_flags & ~3u) != 0u ||
		(REG32(CLOCK_RESET_CONTROL) & (1u << 12)) == 0u) {
		return SECONDARY_BUSY;
	}
	for (i = 0u; i < sizeof(registers) / sizeof(registers[0]); ++i) {
		saved[i] = REG32(registers[i]);
	}
	reset_bit = (REG32(CLOCK_RESET_CONTROL) >> 12) & 1u;
	REG32(SPI_CHANNEL_CONFIG) = 6u;
	REG32(SPI_CONFIG) = 12u;
	REG32(SPI_CLOCK_CONFIG) = 19u;
	REG32(SPI_BASE + 0xCu) = 0u;
	REG32(SPI_INT_MASK) = 0xFFu;
	REG32(SPI_BASE + 0x1Cu) = 0u;
	for (i = 0u; i < sizeof(alternatives) / sizeof(alternatives[0]); ++i) {
		base = alternatives[i][0] ? GPIOB_AF_SELECT : GPIOA_AF_SELECT;
		bit = 1u << alternatives[i][1];
		option = alternatives[i][2] - 1u;
		if ((REG32(base) & bit) != 0u &&
			((REG32(base + 4u) & bit) != 0u) == ((option & 2u) != 0u) &&
			((REG32(base + 8u) & bit) != 0u) == ((option & 1u) != 0u)) {
			REG32(base) &= ~bit;
		}
	}
	REG32(GPIOA_AF_SELECT_1) |= SECBOOT_SPI_PINS;
	REG32(GPIOA_AF_SELECT_0) = (REG32(GPIOA_AF_SELECT_0) | (1u << 9) | (1u << 11)) &
		~((1u << 2) | (1u << 3));
	REG32(GPIOA_AF_SELECT) |= SECBOOT_SPI_PINS;
	while (length != 0u) {
		command_length = opcode == 0x9Fu ? 1u : 4u;
		count = length > 28u ? 28u : length;
		clocks = command_length + count;
		REG32(SPI_CHANNEL_CONFIG) = 6u | (1u << 22);
		if (!spi_wait(SPI_CHANNEL_CONFIG, 1u << 22, 0u)) {
			goto cleanup;
		}
		for (i = 0u; i < clocks; i += 4u) {
			word = 0u;
			for (j = 0u; j < 4u && i + j < clocks; ++j) {
				bit = i + j;
				if (bit == 0u) {
					byte = opcode;
				} else if (bit < command_length) {
					byte = (offset >> ((3u - bit) * 8u)) & 0xFFu;
				} else {
					byte = 0xFFu;
				}
				word |= byte << ((3u - j) * 8u);
			}
			REG32(SPI_TX_DATA) = word;
		}
		REG32(SPI_CHANNEL_CONFIG) = (command_length * 8u << 23) | (1u << 21) | (1u << 20) |
			(1u << 19) | (clocks * 8u << 3) | 3u;
		if (!spi_wait(SPI_STATUS, SPI_BUSY | 0xFC0u, count << 6) ||
			(REG32(SPI_INT_STATUS) & 0xBCu) != 0u) {
			goto cleanup;
		}
		REG32(SPI_CHANNEL_CONFIG) = 6u;
		for (i = 0u; i < count; ++i) {
			if ((i & 3u) == 0u) {
				word = REG32(SPI_RX_DATA);
			}
			*data++ = (uint8_t)(word >> ((3u - (i & 3u)) * 8u));
		}
		offset += count;
		length -= count;
	}
	ok = 1;
cleanup:
	REG32(SPI_CHANNEL_CONFIG) = 6u;
	if (!spi_wait(SPI_STATUS, SPI_BUSY, 0u)) {
		bitband_write(CLOCK_RESET_CONTROL, 12u, 0u);
		bitband_write(CLOCK_RESET_CONTROL, 12u, 1u);
	}
	if (!spi_wait(SPI_STATUS, SPI_BUSY, 0u)) {
		return SECONDARY_CLEANUP_FAILED;
	}
	REG32(SPI_CHANNEL_CONFIG) = 6u | (1u << 22);
	if (!spi_wait(SPI_CHANNEL_CONFIG, 1u << 22, 0u)) {
		bitband_write(CLOCK_RESET_CONTROL, 12u, 0u);
		bitband_write(CLOCK_RESET_CONTROL, 12u, 1u);
		if (!spi_wait(SPI_STATUS, SPI_BUSY | SPI_FIFO_COUNTS, 0u)) {
			return SECONDARY_CLEANUP_FAILED;
		}
	}
	REG32(SPI_INT_STATUS) = REG32(SPI_INT_STATUS) & ~initial_flags & 0xFFu;
	if (pending_bit != 0u) {
		REG32(0xE000E200u) = 1u << 21;
	} else {
		REG32(NVIC_CLEAR_PENDING_0) = 1u << 21;
	}
	for (i = sizeof(registers) / sizeof(registers[0]); i != 0u; --i) {
		if (i == 6u) {
			REG32(GPIOA_AF_SELECT) &= ~SECBOOT_SPI_PINS;
		}
		REG32(registers[i - 1u]) = saved[i - 1u];
	}
	bitband_write(CLOCK_RESET_CONTROL, 12u, reset_bit);
	return ok ? SECONDARY_SUPPORTED : SECONDARY_TIMEOUT;
}

void stub_flash_read(void* dest, uint32_t off, size_t len)
{
	//memcpy(dest, (void*)FLASH_BASE + off, len);
	if(off < primary_size)
	{
		flash_read(FLASH_BASE + off, dest, len);
	}
	else if(secondary_state == SECONDARY_SUPPORTED)
	{
		secondary_read(0x03, off - primary_size, dest, len);
	}
}
static void secondary_probe(void)
{
	uint8_t id[3], check[3];
	uint32_t i;
	secondary_size = 0u;
	secondary_id = 0u;
	secondary_initial_state[0] = REG32(SPI_STATUS);
	secondary_initial_state[1] = REG32(SPI_INT_STATUS);
	secondary_initial_state[2] = REG32(0xE000E200u) & (1u << 21);
	secondary_initial_state[3] = REG32(CLOCK_RESET_CONTROL);
	secondary_state = (uint32_t)secondary_read(0x9Fu, 0u, id, 3u);
	if (secondary_state != SECONDARY_SUPPORTED) {
		return;
	}
	secondary_state = (uint32_t)secondary_read(0x9Fu, 0u, check, 3u);
	if (secondary_state != SECONDARY_SUPPORTED) {
		return;
	}
	for (i = 0u; i < 3u; ++i) {
		secondary_id |= (uint32_t)id[i] << (i * 8u);
		if (id[i] != check[i]) {
			secondary_state = SECONDARY_UNSTABLE;
			return;
		}
	}
	if (secondary_id == 0u || secondary_id == 0xFFFFFFu) {
		secondary_state = SECONDARY_NO_RESPONSE;
	} else if (id[0] == 0u || id[0] == 0xFFu || id[2] != 0x14u) {
		secondary_state = SECONDARY_UNSUPPORTED;
	} else {
		secondary_size = FLASH_SIZE;
	}
}

void boot_from_flash(void)
{
    /* The reader executes entirely from SRAM. Arm a reset, erase only the
       temporary secboot sector, and leave the immutable Mask ROM in control. */
    while ((REG32(UART0_FIFO_STATUS) & UART_TX_COUNT) != 0u) {
    }

    REG32(WATCHDOG_LOCK) = 0x1ACCE551u;
    REG32(WATCHDOG_LOAD) = 40000000u;
    REG32(WATCHDOG_CONTROL) = 3u;
    REG32(WATCHDOG_LOCK) = 1u;

    REG32(FLASH_COMMAND) = 0x00000006u;
    REG32(FLASH_ADDRESS) = 0x10000000u;
    REG32(FLASH_COMMAND) = 0x80000820u;
    REG32(FLASH_ADDRESS) = 0x10200000u;

    for (;;) {
    }
}

void flash_init(void)
{
	flash_id = flash_read_jedec_id();
	primary_size = flash_size = flash_size_from_jedec(flash_id);
	//secondary_probe();
	//if(secondary_state == SECONDARY_SUPPORTED)
	//{
	//	flash_size += flash_size_from_jedec(secondary_id);
	//}
}

void flash_program_page(uint32_t off, const uint8_t* data)
{

}

int flash_erase_range(uint32_t src, uint32_t len)
{

}

int flash_erase_chip()
{

}

extern uint32_t Crc32Table[256];
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

uint8_t get_chip_data(void)
{
	WRITE_REG32(cmd_buf, 0x01020304);
	//WRITE_REG32(cmd_buf, 0xAEE7777A); // W600
	WRITE_REG32(cmd_buf + 4, secondary_state);
	WRITE_REG32(cmd_buf + 8, secondary_id);
	return 12;
}

extern int main(void);

void loader_main(uint32_t baud_register)
{
	REG32(WATCHDOG_CONTROL) = 0u;
	REG32(WATCHDOG_INT_CLR) = 1u;
	extern uint8_t __bss_start__[];
	extern uint8_t __bss_end__[];
	memset((void*)__bss_start__, 0, (__bss_end__ - __bss_start__));
	baud_reg = baud_register;
	main();
}
