#include "../hal_generic.h"

void uart_putc(uint8_t b)
{
	while(hal_uart_flag_get(UART0_BASE, USART_FLAG_TXE) == HAL_RESET);
	hal_uart_send_data(UART0_BASE, b);
}

int8_t uart_getc_timeout(uint8_t* out, uint32_t loops)
{
	while(loops--)
	{
		if(hal_uart_flag_get(UART0_BASE, USART_FLAG_RXNE) == HAL_SET)
		{
			*out = (uint8_t)hal_uart_recv_data(UART0_BASE);
			return 1;
		}
	}
	return 0;
}

void uart_set_baud(uint32_t baud)
{
	hal_uart_baudrate_set(UART0_BASE, baud);
}

void uart_init(void)
{
	hal_gpio_pin_afio_select(GPIOA_BASE, GPIO_PIN_3, UART0_RX);
	hal_gpio_pin_afio_select(GPIOA_BASE, GPIO_PIN_2, UART0_TX);
	hal_gpio_pin_afio_en(GPIOA_BASE, GPIO_PIN_2, HAL_ENABLE);
	hal_gpio_pin_afio_en(GPIOA_BASE, GPIO_PIN_3, HAL_ENABLE);

	uart_init_t_def uart_init_struct;
	memset(&uart_init_struct, 0, sizeof(uart_init_struct));

	uart_init_struct.baudrate = 115200;
	uart_init_struct.parity = UART_PARITY_NONE;
	uart_init_struct.stop_bits = UART_STOP_BITS_1;
	uart_init_struct.word_len = UART_WORD_LEN_8;
	uart_init_struct.over_sampl = UART_OVER_SAMPL_8;

	hal_uart_init(UART0_BASE, &uart_init_struct);
	hal_uart_rx_mode_en(UART0_BASE, HAL_ENABLE);
	hal_uart_tx_mode_en(UART0_BASE, HAL_ENABLE);
	hal_uart_en(UART0_BASE, HAL_ENABLE);

	uart_putc(0x06);
}

void flash_program_page(uint32_t off, const uint8_t* data)
{
	uint8_t page_program_buf[4 + FLASH_PAGE_SIZE];
	page_program_buf[0] = FLASH_PAGE_PROGRAM;
	page_program_buf[1] = (off >> 16) & 0xff;
	page_program_buf[2] = (off >> 8) & 0xff;
	page_program_buf[3] = (off) & 0xff;
	memcpy(&page_program_buf[4], data, FLASH_PAGE_SIZE);
	flash_cache_disable();
	hal_flash_write_enable();
	hal_qspi_standard_write(page_program_buf, sizeof(page_program_buf));
	hal_flash_operation_wait();
	flash_cache_init(0);
}

int flash_erase_range(uint32_t off, uint32_t len)
{
	flash_cache_disable();
	hal_flash_erase(off, len);
	flash_cache_init(0);
	return 1;
}

int flash_erase_chip()
{
	flash_cache_disable();
	hal_flash_chip_erase();
	flash_cache_init(0);
	return 0;
}

void flash_init(void)
{
	hal_flash_init();
	uint32_t flash_ID = hal_flash_read_id();
	flash_id = ((uint8_t*)&flash_ID)[2] | (((uint8_t*)&flash_ID)[1] << 8) | (((uint8_t*)&flash_ID)[0] << 16);
	hal_flash_quad_mode_enable(1);
	flash_cache_init(0);
}

int crc32_memory(uint32_t addr, uint32_t len, uint32_t* result)
{
	*result = ln_crc32_signle_cal((uint8_t*)addr, len);
	return 1;
}

uint16_t crc16_xmodem(const uint8_t* data, uint32_t len)
{
	return crc16_ccitt(data, len);
}

int read_efuse(void)
{
	for(int i = 0; i < 8; ++i)
	{
		cmd_buf[i] = hal_efuse_read_shadow_reg(i);
	}
	for(int i = 8; i < 16; ++i)
	{
		cmd_buf[i] = hal_efuse_read_corrent_reg(i - 8);
	}
	return 64;
}

void get_chip_data(void)
{
	WRITE_REG32(cmd_buf, 0xA40B7429); // LN882H
}

int read_otp(void)
{
	hal_flash_security_area_read(0, 0x400, (uint8_t*)&cmd_buf);
	return 0x400;
}

extern int main(void);

void Reset_Handler(void)
{
	__asm volatile("cpsid i");
#ifdef __ARM_FP
	OR_REG32(0xE000ED88, 0xF << 20);
#endif
	extern uint8_t __bss_start__[];
	extern uint8_t __bss_end__[];
	memset((void*)__bss_start__, 0, (__bss_end__ - __bss_start__));
	clock_init_t clock_init;
	memset((void*)(&clock_init), 0, sizeof(clock_init));
	clock_init.clk_src = CLK_SRC_PLL;
	clock_init.clk_pclk0_div = CLK_PCLK0_4_DIV;
	clock_init.clk_hclk_div = CLK_HCLK_NO_DIV;
	clock_init.clk_pllclk_mul = CLK_PLL_CLK_4_MUL; // can be set higher to overclock.
	hal_misc_awo_set_r_vtrim(1);
	hal_misc_awo_set_r_xtal40m_ldo_vbit(3);
	hal_misc_awo_set_r_xtal40m_cap_bit(0);
	hal_misc_awo_set_r_xtal40m_ibsel(0);
	hal_misc_awo_set_ldo18_4_vset(7);
	hal_misc_awo_set_ldo18_2_vset(7);
	hal_misc_awo_set_ldo15_1_vset(7);
	
	hal_misc_awo_set_o_cpu_sleep_counter_bp(0);
	hal_misc_awo_set_bod_vth(0x01);
	
	hal_clock_init(&clock_init);
	main();
}

void Default_Handler(void)
{
	while(1);
}

const void* __Vectors[240] __attribute__((section(".vectors"))) = {
	(void*)(&__StackTop),                     /* (0x00)Top of Stack                   */
	Reset_Handler,                            /* (0x04)IRQ -15  Reset Handler         */
	Default_Handler,                          /* (0x08)IRQ -14  NMI Handler           */
	Default_Handler,                          /* (0x0C)IRQ -13  Hard Fault Handler    */
	Default_Handler,                          /* (0x10)IRQ -12  MPU Fault Handler     */
	Default_Handler,                          /* (0x14)IRQ -11  Bus Fault Handler     */
	Default_Handler,                          /* (0x18)IRQ -10  Usage Fault Handler   */
	0,                                        /* (0x1C)IRQ -9   Reserved              */
	0,                                        /* (0x20)IRQ -8   Reserved              */
	0,                                        /* (0x24)IRQ -7   Reserved              */
	0,                                        /* (0x28)IRQ -6   Reserved              */
	Default_Handler,                          /* (0x2C)IRQ -5   SVCall Handler        */
	Default_Handler,                          /* (0x30)IRQ -4   Debug Monitor Handler */
	0,                                        /* (0x34)IRQ -3   Reserved              */
	Default_Handler,                          /* (0x38)IRQ -2   PendSV Handler        */
	Default_Handler,                          /* (0x3C)IRQ -1   SysTick Handler       */

	/* Interrupts */
	Default_Handler,                          /* (0x40)IRQ0  */
	Default_Handler,                          /* (0x44)IRQ1  */
	Default_Handler,                          /* (0x48)IRQ2  */
	Default_Handler,                          /* (0x4C)IRQ3  */
	Default_Handler,                          /* (0x50)IRQ4  */
	Default_Handler,                          /* (0x54)IRQ5  */
	Default_Handler,                          /* (0x58)IRQ6  */
	Default_Handler,                          /* (0x5C)IRQ7  */
	Default_Handler,                          /* (0x60)IRQ8  */
	Default_Handler,                          /* (0x64)IRQ9  */
	Default_Handler,                          /* (0x68)IRQ10 */
	Default_Handler,                          /* (0x6C)IRQ11 */
	Default_Handler,                          /* (0x70)IRQ12 */
	Default_Handler,                          /* (0x74)IRQ13 */
	Default_Handler,                          /* (0x78)IRQ14 */
	Default_Handler,                          /* (0x7C)IRQ15 */
	Default_Handler,                          /* (0x80)IRQ16 */
	Default_Handler,                          /* (0x84)IRQ17 */
	Default_Handler,                          /* (0x88)IRQ18 */
	Default_Handler,                          /* (0x8C)IRQ19 */
	Default_Handler,                          /* (0x90)IRQ20 */
	Default_Handler,                          /* (0x94)IRQ21 */
	Default_Handler,                          /* (0x98)IRQ22 */
	Default_Handler,                          /* (0x9C)IRQ23 */
	Default_Handler,                          /* (0xA0)IRQ24 */
	Default_Handler,                          /* (0xA4)IRQ25 */
	Default_Handler,                          /* (0xA8)IRQ26 */
	Default_Handler,                          /* (0xAC)IRQ27 */
	Default_Handler,                          /* (0xB0)IRQ28 */
	Default_Handler,                          /* (0xB4)IRQ29 */
	Default_Handler,                          /* (0xB8)IRQ30 */
	Default_Handler,                          /* (0xBC)IRQ31 */
	Default_Handler,                          /* (0xC0)IRQ32 */
	Default_Handler,                          /* (0xC4)IRQ33 */
	Default_Handler,                          /* (0xC8)IRQ34 */
	Default_Handler,                          /* (0xCC)IRQ35 */
	Default_Handler,                          /* (0xD0)IRQ36 */
	Default_Handler,                          /* (0xD4)IRQ37 */
};


#pragma pack(4)
typedef struct
{
	uint8_t*  bootram_target_addr; // bootram load addr
	uint16_t  bootram_bin_length;  // Tools build
	uint16_t  bootram_crc_offset;  // >= (sizeof(boot_header_t) + sizeof(boot_header_ext_t))
	uint32_t  bootram_crc_value;   // Tools build (Depends on the bootram_crc_offset.)
	uint32_t* bootram_vector_addr; // bootram vector addr (At least 256 Byte aligned.)
	uint32_t  crp_flag;            // Tools build
	uint32_t  boot_hearder_crc;    // Tools build
} boot_header_t;
#pragma pack()

__attribute__((section(".boot_header"), used)) const boot_header_t boot_header = {
	.bootram_target_addr = (uint8_t*)BOOTRAM_BASE,
	.bootram_bin_length = 0,
	.bootram_crc_offset = 0,
	.bootram_crc_value = 0x0,
	.bootram_vector_addr = (uint32_t*)&__Vectors,
	.crp_flag = 0x0,
	.boot_hearder_crc = 0,
};
