#include "../hal_generic.h"

uint32_t stub_uart = USART0;
__attribute__((aligned(4)));
static uint8_t alloc_buf[0x3000];
struct rom_api_t* p_rom_api = (struct rom_api_t*)ROM_API_ARRAY_BASE;
void irq_entry() {}
void exc_entry() {}

void uart_putc(uint8_t b)
{
	usart_data_transmit(stub_uart, b);
	while(RESET == usart_flag_get(stub_uart, USART_FLAG_TBE));
}

int8_t uart_getc_timeout(uint8_t* out, uint32_t loops)
{
	while(loops--)
	{
		if(RESET != usart_flag_get(stub_uart, USART_FLAG_ORERR))
		{
			usart_flag_clear(stub_uart, USART_FLAG_ORERR);
		}

		if(RESET != usart_flag_get(stub_uart, USART_FLAG_RBNE))
		{
			*out = (char)usart_data_receive(stub_uart);
			return 1;
		}
	}
	return 0;
}

void uart_set_baud(uint32_t baud)
{
	usart_disable(stub_uart);
	usart_baudrate_set(stub_uart, baud);
	usart_enable(stub_uart);
}

static void uart_init_gd(uint32_t usart_periph)
{
	if(usart_periph == UART2)
	{
		rcu_periph_clock_enable(RCU_UART2);
		rcu_periph_clock_enable(RCU_GPIOA);
		gpio_af_set(GPIOA, GPIO_AF_10, GPIO_PIN_6);  // UART2 TX
		gpio_af_set(GPIOA, GPIO_AF_8, GPIO_PIN_7);   // UART2 RX
		gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
		gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_6);
		gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
		gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_7);
	}
	else if(usart_periph == UART1)
	{
		rcu_periph_clock_enable(RCU_GPIOA);
		rcu_periph_clock_enable(RCU_UART1);
		gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_4);   // UART1 TX
		gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_5);   // UART1 RX
		gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
		gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_4);
		gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
		gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_5);
	}
	else if(usart_periph == USART0)
	{
		rcu_periph_clock_enable(RCU_USART0);
		rcu_periph_clock_enable(RCU_GPIOA);
		rcu_periph_clock_enable(RCU_GPIOB);
		gpio_af_set(GPIOA, GPIO_AF_2, GPIO_PIN_8);   // UART0 TX
		gpio_af_set(GPIOB, GPIO_AF_8, GPIO_PIN_15);  // UART0 RX
		gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
		gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_8);
		gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_15);
		gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_15);
	}
	else
	{
		return;
	}

	usart_deinit(usart_periph);
	usart_baudrate_set(usart_periph, 115200U);
	usart_receive_config(usart_periph, USART_RECEIVE_ENABLE);
	usart_transmit_config(usart_periph, USART_TRANSMIT_ENABLE);
	usart_receive_fifo_enable(usart_periph);
	usart_enable(usart_periph);
}

void uart_init(void)
{
	uart_init_gd(USART0);
	uart_init_gd(UART1);
	uart_init_gd(UART2);

	while(1)
	{
		char ch = 0;
		stub_uart = USART0;
		if(uart_getc_timeout(&ch, 1000) == 0)
		{
			break;
		}
		stub_uart = UART1;
		if(uart_getc_timeout(&ch, 1000) == 0)
		{
			break;
		}
		stub_uart = UART2;
		if(uart_getc_timeout(&ch, 1000) == 0)
		{
			break;
		}
	}
}

void flash_init(void)
{
	flash_id = 0x00160000;
}

void flash_program_page(uint32_t off, const uint8_t* data)
{
	fmc_unlock();
	fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR);
	fmc_continuous_program(FLASH_BASE + off, (uint32_t*)data, FLASH_PAGE_SIZE);
	fmc_lock();
}

int flash_erase_range(uint32_t src, uint32_t len)
{
	rom_flash_erase(src, len);
	return 1;
}

int flash_erase_chip()
{
	fmc_unlock();
	fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR);
	fmc_mass_erase();
	fmc_lock();
	return 1;
}

int crc32_memory_hardware(uint32_t addr, uint32_t len, uint32_t* result)
{
	return 0;
}

int sha256_memory_hardware(uint32_t addr, uint32_t len)
{
	unsigned char hash[32] = { 0 };
	hau_hash_sha_256((void*)(addr), len, hash);
	memcpy(cmd_buf, hash, 32);
	return 1;
}

void rf_efuse_map_get(uint8_t map[64])
{
	uint8_t raw[64];

	for(int i = 0; i < 64; i++)
		raw[i] = RF_EFUSE_BASE[i];

	memset(map, 0, 64);

	int pos = 0;

	while(pos < 64)
	{
		uint8_t hdr = raw[pos++];

		if(hdr == 0xFF)
			break;

		uint8_t block = hdr >> 4;
		uint8_t bitmap = hdr & 0x0F;

		for(int i = 0; i < 4; i++)
		{
			if(bitmap & (1U << i))
			{
				if(pos >= 64)
					return;

				uint8_t index = block * 4 + i;

				if(index < 64)
					map[index] = raw[pos++];
			}
		}
	}
}

int read_efuse(void)
{
	uint8_t raw[64];

	for(uint8_t i = 0; i < 64; i++) raw[i] = RF_EFUSE_BASE[i];

	memset(cmd_buf, 0, 64);

	int pos = 0;

	while(pos < 64)
	{
		uint8_t hdr = raw[pos++];

		if(hdr == 0xFF)
			break;

		uint8_t block = hdr >> 4;
		uint8_t bitmap = hdr & 0x0F;

		for(int i = 0; i < 4; i++)
		{
			if(bitmap & (1U << i))
			{
				if(pos >= 64)
					return 64;

				uint8_t index = block * 4 + i;

				if(index < 64)
					cmd_buf[index] = raw[pos++];
			}
		}
	}
	return 64;
}

int read_factory_mac(uint8_t mac[6])
{
	return 0;
}

void get_chip_data(void)
{
	memset(cmd_buf, 0, 16);
	WRITE_REG32(cmd_buf, 0xFFDC26B5); // GD32VW553
	WRITE_REG32(cmd_buf + 4, FMC_PID0);
	WRITE_REG32(cmd_buf + 8, FMC_PID1);
}

void _premain_init(void)
{
	rom_symbol_init();
	mbedtls_memory_buffer_alloc_init(alloc_buf, sizeof(alloc_buf));
	rcu_periph_clock_enable(RCU_HAU);
	rcu_periph_clock_enable(RCU_CRC);
	rcu_periph_clock_enable(RCU_DMA);
	rom_digest_haudma_en(1);
	mbedtls_platform_set_hardware_poll(rom_hardware_poll);
	mbedtls_ecp_curve_val_init();
	int mstatus;
	__asm volatile ("csrrci %0, mstatus, %1" : "=r" (mstatus) : "i" (1 << 3));
}

void _postmain_fini(int status)
{

}