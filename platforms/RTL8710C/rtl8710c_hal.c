#include "../hal_generic.h"

hal_uart_adapter_t log_uart;
hal_spic_adaptor_t hal_spic_adaptor;

void uart_putc(uint8_t b)
{
	while(UART2->tflvr_b.tx_fifo_lv >= Uart_Tx_FIFO_Size);
	UART2->thr = b;
}

int8_t uart_getc_timeout(uint8_t* out, uint32_t loops)
{
	while(loops--)
	{
		if(UART2->rflvr_b.rx_fifo_lv > 0)
		{
			*out = UART2->rbr;
			return 1;
		}
	}
	return 0;
}

void uart_set_baud(uint32_t baud)
{
	hal_uart_set_baudrate(&log_uart, baud);
}

void uart_init(void)
{
	hal_uart_defconfig_t cfg =
	{
		.baudrate = 115200,
		.flow_ctrl = UartFlowCtlNone,
		.word_len = 8,
		.stop_bit = 1,
		.parity_type = 0
	};
	hal_uart_stubs.hal_uart_init(&log_uart, 16, 15, &cfg);
}

u8 hal_flash_get_status(phal_spic_adaptor_t phal_spic_adaptor, u8 cmd)
{
	return hal_flash_stubs.hal_flash_get_status(phal_spic_adaptor, cmd);
}

void hal_flash_set_status(phal_spic_adaptor_t phal_spic_adaptor, u8 cmd, u8 data)
{
	hal_flash_stubs.hal_flash_set_status(phal_spic_adaptor, cmd, data);
}

void spic_tx_cmd(phal_spic_adaptor_t phal_spic_adaptor, u8 cmd, u8 data_phase_len, u8* pdata)
{
	hal_spic_stubs.spic_tx_cmd(phal_spic_adaptor, cmd, data_phase_len, pdata);
}

void hal_flash_set_write_enable(phal_spic_adaptor_t phal_spic_adaptor)
{
	hal_flash_stubs.hal_flash_set_write_enable(phal_spic_adaptor);
}

void hal_flash_enable_qpi(phal_spic_adaptor_t phal_spic_adaptor)
{
	hal_flash_stubs.hal_flash_enable_qpi(phal_spic_adaptor);
}

void hal_flash_set_quad_enable(phal_spic_adaptor_t phal_spic_adaptor)
{
	pflash_cmd_t cmd = phal_spic_adaptor->cmd;
	u8 flash_type = phal_spic_adaptor->flash_type;
	u8 status_value = 0;
	u8 data[2];

	switch(flash_type)
	{
		case FLASH_TYPE_WINBOND:
		case FLASH_TYPE_GD32:
		case FLASH_TYPE_BOYA:
		case FLASH_TYPE_XMC:
		case FLASH_TYPE_PUYA:
		case FLASH_TYPE_ZBIT:
		case FLASH_TYPE_GTEC:
			status_value = hal_flash_get_status(phal_spic_adaptor, cmd->rdsr2);
			if((status_value & 0x2) == 0)
			{
				hal_flash_set_status(phal_spic_adaptor, cmd->wrsr2, 0x2 | status_value);
			}
			break;

		case FLASH_TYPE_MXIC:
			status_value = hal_flash_get_status(phal_spic_adaptor, cmd->rdsr);
			if((status_value & 0x40) == 0)
			{
				hal_flash_set_status(phal_spic_adaptor, cmd->wrsr, 0x40 | status_value);
			}
			break;

		case FLASH_TYPE_GD:
		case FLASH_TYPE_XTX:
		case FLASH_TYPE_TSTE:
			data[1] = hal_flash_get_status(phal_spic_adaptor, cmd->rdsr2);
			if((data[1] & 0x2) == 0)
			{
				data[0] = hal_flash_get_status(phal_spic_adaptor, cmd->rdsr);
				data[1] |= 0x2;
				hal_flash_set_write_enable(phal_spic_adaptor);
				spic_tx_cmd(phal_spic_adaptor, cmd->wrsr, 2, data);
			}
			break;

		case FLASH_TYPE_EON:
			break;

		default:
			DBG_SPIF_ERR("Unknown flash type, cannot set QE bit!\n");
	}
}

void spic_config_auto_mode(phal_spic_adaptor_t phal_spic_adaptor)
{
	SPIC_Type* spic_dev = phal_spic_adaptor->spic_dev;
	pflash_cmd_t cmd = phal_spic_adaptor->cmd;
	spic_valid_cmd_t valid_cmd;
	u8 spic_bit_mode = phal_spic_adaptor->spic_bit_mode;
	u8 flash_type = phal_spic_adaptor->flash_type;


	/*Set default send cmd channel mode*/
	phal_spic_adaptor->spic_send_cmd_mode = SingleChnl;

	spic_disable_rtl8710c(spic_dev);
	valid_cmd.w = 0;
	valid_cmd.b.wr_blocking = ENABLE;
	valid_cmd.b.prm_en = ENABLE;

	spic_dev->valid_cmd = valid_cmd.w;

	switch(spic_bit_mode)
	{
		case SpicOneIOMode:
			spic_dev->read_fast_single = cmd->fread;
			break;

		case SpicDualOutputMode:        /*Enable 1-1-2*/
			spic_dev->read_dual_data = cmd->dread;
			spic_dev->valid_cmd_b.rd_dual_i = ENABLE;
			break;

		case SpicDualIOMode:            /*Enable 1-2-2*/
			spic_dev->read_dual_addr_data = cmd->str_2read;
			spic_dev->valid_cmd_b.rd_dual_io = ENABLE;
			break;

		case SpicQuadOutputMode:        /*Enable 1-1-4*/
			spic_dev->read_quad_data = cmd->qread;
			spic_dev->valid_cmd_b.rd_quad_o = ENABLE;
			if((flash_type == FLASH_TYPE_WINBOND)
				|| (flash_type == FLASH_TYPE_GD)
				|| (flash_type == FLASH_TYPE_GD32)
				|| (flash_type == FLASH_TYPE_ZBIT)
				|| (flash_type == FLASH_TYPE_XTX)
				|| (flash_type == FLASH_TYPE_GTEC)
				|| (flash_type == FLASH_TYPE_TSTE))
			{
				hal_flash_set_quad_enable(phal_spic_adaptor);
			}
			break;

		case SpicQuadIOMode:            /*Enable 1-4-4*/
			spic_dev->read_quad_addr_data_b.rd_quad_io_cmd = cmd->str_4read;
			spic_dev->valid_cmd_b.rd_quad_io = ENABLE;
			hal_flash_set_quad_enable(phal_spic_adaptor);
			break;

		case SpicQpiMode:               /*Enable 4-4-4*/
			spic_dev->read_quad_addr_data_b.rd_quad_io_cmd = cmd->str_4read;
			spic_dev->valid_cmd_b.rd_quad_io = ENABLE;
			hal_flash_enable_qpi(phal_spic_adaptor);
			break;

		default:
			DBG_SPIF_ERR("spic_config_auto_mode_rtl8710c : Invalid Bit Mode\r\n");
	}

	phal_spic_adaptor->addr_byte_num = ThreeBytesLength;

	spic_dev->auto_length_b.auto_addr_length = phal_spic_adaptor->addr_byte_num;
	spic_enable_rtl8710c(spic_dev);
}

void flash_init(void)
{
	memset(&hal_spic_adaptor, 0, sizeof(hal_spic_adaptor));
	//uint32_t boot_cfg = READ_REG32(0x40000038);
	//uint8_t boot_mode = (boot_cfg & 0x10) ? SpicQuadIOMode : SpicOneIOMode;
	//uint8_t pin_sel = (boot_cfg >> 5) & 0x3;
	//spic_init(&hal_spic_adaptor, boot_mode, &spic_flash_pins[pin_sel]);
	spic_config_auto_mode(&hal_spic_adaptor);
	hal_flash_stubs.hal_flash_read_id(&hal_spic_adaptor);
	memcpy(&flash_id, hal_spic_adaptor.flash_id, 3);
}

void flash_program_page(uint32_t off, const uint8_t* data)
{
	hal_flash_stubs.hal_flash_page_program(&hal_spic_adaptor, FLASH_PAGE_SIZE, off, (uint8_t*)data);
	dcache_invalidate_by_addr((void*)off, FLASH_PAGE_SIZE);
}

int flash_erase_range(uint32_t src, uint32_t len)
{
	if((src & 0x3) != 0)
		return 0;
	if(len == 0)
		return 0;

	uint32_t cur = src;
	uint32_t end = src + len;

	while(cur < end)
	{
		if((cur % 0x10000U == 0) && (end - cur >= 0x10000U))
		{
			hal_flash_stubs.hal_flash_64k_block_erase(&hal_spic_adaptor, cur);
			cur += 0x10000U;
		}
		else
		{
			hal_flash_stubs.hal_flash_sector_erase(&hal_spic_adaptor, cur);
			cur += 0x1000U;
		}
	}

	dcache_invalidate_by_addr((void*)src, len);

	return 1;
}

int flash_erase_chip()
{
	hal_flash_stubs.hal_flash_chip_erase(&hal_spic_adaptor);
	return 1;
}

int crc32_memory_hardware(uint32_t addr, uint32_t len, uint32_t* result)
{
	return 0;
}

int sha256_memory_hardware(uint32_t addr, uint32_t len)
{
	__attribute__((aligned(8))) uint8_t hash[32] = { 0 };
	hal_crypto_sha2_256_init();
	uint32_t remaining = len;
	while(remaining > 0)
	{
		uint32_t chunk = remaining > 0x10000 ? 0x10000 : remaining;
		hal_crypto_sha2_256_update((void*)addr, chunk);
		addr += chunk;
		remaining -= chunk;
	}
	hal_crypto_sha2_256_final((uint8_t*)&hash);
	memcpy(cmd_buf, hash, 32);
	return 1;
}

int read_efuse(void)
{
	memset(cmd_buf, 0, 0x200);
	for(uint16_t addr = 0; addr < 0x200; addr++)
	{
		uint32_t(*read)(uint32_t ctrl_setting, uint16_t addr, uint8_t* data, uint8_t ldo_voltage);
		if((addr < 0x130) || (addr >= (0x130 + 0x80)))
		{
			read = hal_efuse_stubs.hal_efuse_read;
		}
		else
		{
			read = hal_efuse_stubs.hal_sec_efuse_read;
		}

		read(EFUSE_CTRL_SETTING, addr, cmd_buf + addr, LDO_OUT_DEFAULT_VOLT);
	}
	return 0x200;
}

int read_factory_mac(uint8_t mac[6])
{
	for(uint8_t i = 0; i < 6; i++)
	{
		hal_efuse_stubs.hal_efuse_read(EFUSE_CTRL_SETTING, 0x27 + i, &mac[i], LDO_OUT_DEFAULT_VOLT);
	}
	return 1;
}

void get_chip_data(void)
{
	WRITE_REG32(cmd_buf, 0x34B6B640); // RTL8710C
	hal_efuse_stubs.hal_efuse_read(EFUSE_CTRL_SETTING, 0x1F8, cmd_buf + 4, LDO_OUT_DEFAULT_VOLT);
	WRITE_REG32(cmd_buf + 8, READ_REG32(0x400001F0));
}

#undef memset
#undef memcpy
__attribute__((used))
void* memset(void* dst0, int val, size_t length)
{
	return utility_stubs.memset(dst0, val, length);
}

__attribute__((used))
void* memcpy(void* dest, const void* src, size_t n)
{
	return utility_stubs.memcpy(dest, src, n);
}

extern int main(void);

__attribute__((section(".start"), used))
void flasher_main(void)
{
	__asm volatile ("cpsid i" : : : "memory");
	// 0 - 100mhz, 1 - 50mhz, 2 - 25mhz
	//WRITE_REG32(0x40000250, READ_REG32(0x40000250) & 0xfffcffff | 0 << 0x10);
	memset(__bss_start__, 0, (__bss_end__ - __bss_start__));
	hal_crypto_engine_init();
	main();
}
