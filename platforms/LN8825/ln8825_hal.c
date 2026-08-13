#include "../hal_generic.h"

UART_DevTypeDef UART;

void uart_putc(uint8_t b)
{
	while(!(LL_UART_GetLineStatus(UART0) & LL_UART_LSR_TX_EMPTY));
	LL_UART_WriteChar(UART0, b);
}

int8_t uart_getc_timeout(uint8_t* out, uint32_t loops)
{
	while(loops--)
	{
		if(LL_UART_GetLineStatus(UART0) & LL_UART_LSR_DATA_READY)
		{
			*out = LL_UART_ReadChar(UART0);
			return 1;
		}
	}
	return 0;
}

void uart_set_baud(uint32_t baud)
{
	uint32_t temp_cal = uart_baudrate_cal(APBUS0_CLOCK, baud);
	LL_UART_DLAB_Set(UART0, 1);
	LL_UART_DLL_Set(UART0, (uint8_t)((temp_cal >> 5U) & 0xFF));
	LL_UART_DLH_Set(UART0, (uint8_t)((temp_cal >> 13U) & 0xFF));
	if(temp_cal & 1) LL_UART_DLF_Set(UART0, (uint8_t)(((temp_cal >> 1) & 0x0F) + 1));
	else LL_UART_DLF_Set(UART0, (uint8_t)((temp_cal >> 1) & 0x0F));
	LL_UART_DLAB_Set(UART0, 0);
}

void uart_init(void)
{
	HAL_SYSCON_FuncIOSet(GPIO_AF_UART0_RX, GPIO_AF_IO_18, 1);
	HAL_SYSCON_FuncIOSet(GPIO_AF_UART0_TX, GPIO_AF_IO_19, 1);
	HAL_SYSCON_SoftwareResetPeripheral(SW_RST_UART0);
	LL_UART_En(UART0, 0);
	uart_set_baud(115200);
	LL_UART_Parity_Enable(UART0, 0);
	LL_UART_Stopbits_Set(UART0, UART_STOP_BIT_1);
	LL_UART_DataLenth_Set(UART0, UART_DATALEN_8BIT);
	LL_UART_FlowControl_Set(UART0, UART_FLOW_CONTROL_SOFTWARE);
	LL_UART_En(UART0, 1);
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
	FLASH_WriteEnable();
	qspi_standard_write(page_program_buf, sizeof(page_program_buf));
	FLASH_OperationWait();
	flash_cache_init(0);
}

int flash_erase_range(uint32_t off, uint32_t len)
{
	flash_cache_disable();
	FLASH_Erase(off, len);
	flash_cache_init(0);
	return 1;
}

int flash_erase_chip()
{
	flash_cache_disable();
	FLASH_ChipErase();
	flash_cache_init(0);
	return 0;
}

void flash_init(void)
{
	FLASH_Init();
	FLASH_QuadModeEnable(1);
	uint32_t flash_ID = FLASH_ReadID();
	flash_id = ((uint8_t*)&flash_ID)[2] | (((uint8_t*)&flash_ID)[1] << 8) | (((uint8_t*)&flash_ID)[0] << 16);
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
		cmd_buf[i] = HAL_EFUSE_ReadShadowReg(EFUSE, i);
	}
	for(int i = 8; i < 16; ++i)
	{
		cmd_buf[i] = HAL_EFUSE_ReadCorrectReg(EFUSE, i - 8);
	}
	return 64;
}

void get_chip_data(void)
{
	WRITE_REG32(cmd_buf, 0x8ABF79A8); // LN8825
}

int read_otp(void)
{
	for(int i = 0; i < 0x400; i += 16)
	{
		FLASH_SecurityAreaRead(i, 16, (uint8_t*)&cmd_buf[i]);
	}
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
	HAL_SYSCON_SelectSysClkSrc(SYSCLK_SRC_PLL);
	HAL_SYSCON_SetHclkDivision(SOURCE_CLOCK / AHBUS_CLOCK);
	HAL_SYSCON_SetPclk0Division(AHBUS_CLOCK / APBUS0_CLOCK);
	HAL_SYSCON_SetPclk1Division(AHBUS_CLOCK / APBUS1_CLOCK);
	HAL_SYSCON_32KCaliEnable();
	//HAL_SYSCON_PmuCfg();
	HAL_SYSCON_CPUResetReqMask(0);
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
	.boot_hearder_crc = 0xCB7722EB,
};
