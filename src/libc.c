#ifndef PLATFORM_PROVIDES_LIBC

#include <stddef.h>
#include <stdint.h>

typedef uint32_t alias_u32 __attribute__((may_alias));

__attribute__((used))
void* memcpy(void* dest, const void* src, size_t len)
{
	void* result = dest;
	unsigned char* out = (unsigned char*)dest;
	const unsigned char* in = (const unsigned char*)src;

#ifndef LIGHT_MEMCPY

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

#endif

	while(len--) *out++ = *in++;
	return result;
}

__attribute__((used))
void* memset(void* dest, int value, size_t len)
{
	void* result = dest;
	unsigned char* out = (unsigned char*)dest;

#ifndef LIGHT_MEMSET

	if(len >= 16U)
	{
		while((uintptr_t)out & 3U)
		{
			*out++ = (unsigned char)value;
			len--;
		}

		alias_u32 word = (alias_u32)value * 0x01010101U;
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

#endif

	while(len--) *out++ = (unsigned char)value;
	return result;
}

#endif
