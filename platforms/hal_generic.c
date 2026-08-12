#include "hal_generic.h"

static const uint16_t crc16_table[16] =
{
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
	0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF
};

//int flash_range_is_erased(uint32_t off, uint32_t len)
//{
//	const volatile uint8_t* flash = (const volatile uint8_t*)(uintptr_t)(FLASH_BASE + off);
//	const volatile uint32_t* flash32 =
//		(const volatile uint32_t*)(const volatile void*)flash;
//	while(len >= 16U)
//	{
//		if(flash32[0] != UINT32_MAX || flash32[1] != UINT32_MAX ||
//			flash32[2] != UINT32_MAX || flash32[3] != UINT32_MAX) return 0;
//		flash32 += 4;
//		len -= 16U;
//	}
//	while(len >= 4U)
//	{
//		if(*flash32++ != UINT32_MAX) return 0;
//		len -= 4U;
//	}
//	flash = (const volatile uint8_t*)(const volatile void*)flash32;
//	while(len--) if(*flash++ != 0xFFU) return 0;
//	return 1;
//}
//
//int flash_range_matches(uint32_t off, const uint8_t* data, uint32_t len)
//{
//	const volatile uint8_t* flash = (const volatile uint8_t*)(uintptr_t)(FLASH_BASE + off);
//	if((((uintptr_t)flash | (uintptr_t)data) & 3U) == 0U)
//	{
//		const volatile uint32_t* flash32 =
//			(const volatile uint32_t*)(const volatile void*)flash;
//		const alias_u32* data32 = (const alias_u32*)(const void*)data;
//		while(len >= 16U)
//		{
//			if(flash32[0] != data32[0] || flash32[1] != data32[1] ||
//				flash32[2] != data32[2] || flash32[3] != data32[3]) return 0;
//			flash32 += 4;
//			data32 += 4;
//			len -= 16U;
//		}
//		while(len >= 4U)
//		{
//			if(*flash32++ != *data32++) return 0;
//			len -= 4U;
//		}
//		flash = (const volatile uint8_t*)(const volatile void*)flash32;
//		data = (const uint8_t*)(const void*)data32;
//	}
//	while(len--) if(*flash++ != *data++) return 0;
//	return 1;
//}

int buffer_is_erased(const uint8_t* data, uint32_t len)
{
	const alias_u32* data32 = (const alias_u32*)(const void*)data;
	while(len >= 16U)
	{
		if(data32[0] != UINT32_MAX || data32[1] != UINT32_MAX ||
			data32[2] != UINT32_MAX || data32[3] != UINT32_MAX) return 0;
		data32 += 4;
		len -= 16U;
	}
	while(len >= 4U)
	{
		if(*data32++ != UINT32_MAX) return 0;
		len -= 4U;
	}
	data = (const uint8_t*)(const void*)data32;
	while(len--) if(*data++ != 0xFFU) return 0;
	return 1;
}

uint32_t reverse_u32(uint32_t value)
{
	value = ((value & 0x55555555U) << 1) | ((value >> 1) & 0x55555555U);
	value = ((value & 0x33333333U) << 2) | ((value >> 2) & 0x33333333U);
	value = ((value & 0x0F0F0F0FU) << 4) | ((value >> 4) & 0x0F0F0F0FU);
	value = ((value & 0x00FF00FFU) << 8) | ((value >> 8) & 0x00FF00FFU);
	return (value << 16) | (value >> 16);
}

uint32_t crc32_update_wire(uint32_t crc, uint8_t b)
{
	crc ^= (uint32_t)b;
	for(int i = 0; i < 8; i++)
	{
		crc = (crc & 1U) ? ((crc >> 1) ^ 0xEDB88320U) : (crc >> 1);
	}
	return crc;
}

#ifdef FASTER_CRC32

uint32_t Crc32Table[256] = { 0 };

void crc32_init_table(void)
{
	for(uint32_t i = 0; i < 256; i++)
	{
		uint32_t c = i;
		for(int j = 0; j < 8; j++)
		{
			if((c & 1) != 0)
			{
				c = (0xEDB88320 ^ (c >> 1));
			}
			else
			{
				c = c >> 1;
			}
		}
		Crc32Table[i] = c;
	}
}

__attribute__((optimize("O2"), weak))
int crc32_memory(uint32_t addr, uint32_t len, uint32_t* result)
{
	uint32_t crc = 0xFFFFFFFF;
	const uint8_t* p = (const uint8_t*)addr;
	while(len >= 4)
	{
		crc = (crc >> 8) ^ Crc32Table[(crc ^ *p++) & 0xFF];
		crc = (crc >> 8) ^ Crc32Table[(crc ^ *p++) & 0xFF];
		crc = (crc >> 8) ^ Crc32Table[(crc ^ *p++) & 0xFF];
		crc = (crc >> 8) ^ Crc32Table[(crc ^ *p++) & 0xFF];
		len -= 4;
	}

	while(len--)
		crc = (crc >> 8) ^ Crc32Table[(crc ^ *p++) & 0xFF];

	*result = crc ^ 0xffffffff;
	return 1;
}

#else

void crc32_init_table() { }

__attribute__((weak))
int crc32_memory(uint32_t addr, uint32_t len, uint32_t* result)
{
	uint32_t crc = 0xFFFFFFFFU;
	const volatile uint8_t* p = (const volatile uint8_t*)(uintptr_t)addr;
	for(uint32_t i = 0; i < len; i++)
	{
		crc = crc32_update_wire(crc, p[i]);
	}
	*result = crc ^ 0xFFFFFFFFU;
	return 1;
}

#endif

__attribute__((weak))
uint16_t crc16_xmodem(const uint8_t* data, uint32_t len)
{
	uint16_t crc = 0U;
	while(len--)
	{
		uint8_t byte = *data++;
		crc = (crc << 4) ^ crc16_table[((crc >> 12) ^ (byte >> 4)) & 0x0FU];
		crc = (crc << 4) ^ crc16_table[((crc >> 12) ^ (byte)) & 0x0FU];
	}
	return crc;
}

__attribute__((weak))
int sha256_memory_hardware(uint32_t addr, uint32_t len)
{
	return 0;
}

__attribute__((weak))
void uart_init(void)
{
	uart_set_baud(115200U);
}

__attribute__((weak))
int read_factory_mac(uint8_t mac[6])
{
	return 0;
}

__attribute__((weak))
int read_efuse(void)
{
	return 0;
}

__attribute__((weak))
int read_otp(void)
{
	return 0;
}
