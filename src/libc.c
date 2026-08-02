#ifndef PLATFORM_PROVIDES_LIBC

#include <stddef.h>
#include <stdint.h>

typedef uint32_t alias_u32 __attribute__((may_alias));

void* memcpy(void* dest, const void* src, size_t len)
{
	void* result = dest;
	unsigned char* out = (unsigned char*)dest;
	const unsigned char* in = (const unsigned char*)src;

	if(len >= 16U && (((uintptr_t)out ^ (uintptr_t)in) & 3U) == 0U)
	{
		while(len && ((uintptr_t)out & 3U))
		{
			*out++ = *in++;
			len--;
		}

		alias_u32* out32 = (alias_u32*)(void*)out;
		const alias_u32* in32 = (const alias_u32*)(const void*)in;
		while(len >= 16U)
		{
			out32[0] = in32[0];
			out32[1] = in32[1];
			out32[2] = in32[2];
			out32[3] = in32[3];
			out32 += 4;
			in32 += 4;
			len -= 16U;
		}
		while(len >= 4U)
		{
			*out32++ = *in32++;
			len -= 4U;
		}

		out = (unsigned char*)(void*)out32;
		in = (const unsigned char*)(const void*)in32;
	}

	while(len--) *out++ = *in++;
	return result;
}

void* memset(void* dest, int value, size_t len)
{
	void* result = dest;
	unsigned char* out = (unsigned char*)dest;
	unsigned char byte = (unsigned char)value;

	if(len >= 16U)
	{
		while((uintptr_t)out & 3U)
		{
			*out++ = byte;
			len--;
		}

		alias_u32 word = (alias_u32)byte * 0x01010101U;
		alias_u32* out32 = (alias_u32*)(void*)out;
		while(len >= 16U)
		{
			out32[0] = word;
			out32[1] = word;
			out32[2] = word;
			out32[3] = word;
			out32 += 4;
			len -= 16U;
		}
		while(len >= 4U)
		{
			*out32++ = word;
			len -= 4U;
		}

		out = (unsigned char*)(void*)out32;
	}

	while(len--) *out++ = byte;
	return result;
}

#endif
