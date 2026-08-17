#include "stub_miniz.h"
#include "miniz.h"
#include "../platforms/hal_generic.h"

typedef struct
{
	miniz_put_buffer_fn put_buffer;
	void* ctx;
} miniz_output_t;
#ifndef NO_MINIZ_COMPRESSION
#if PLATFORM_RDA5981 || PLATFORM_ECR6600 || PLATFORM_TR6260
#if PLATFORM_TR6260
__attribute__((section(".dram1")))
#endif
mz_uint16 m_next[TDEFL_LZ_DICT_SIZE];
__attribute__((section(".dram2")))
#elif PLATFORM_OPL1000A2
__attribute__((section(".dram")))
#endif
static tdefl_compressor deflator;
#endif
#ifndef NO_MINIZ_DECOMPRESSION
static tinfl_decompressor inflator;
#ifdef NO_MINIZ_COMPRESSION
static uint8_t inflate_dictionary[TINFL_LZ_DICT_SIZE];
#endif
#endif

#ifndef NO_MINIZ_COMPRESSION
static mz_bool deflate_output(const void* buffer, int len, void* user)
{
	miniz_output_t* output = (miniz_output_t*)user;
	return len <= 0 || output->put_buffer(
		output->ctx, (const uint8_t*)buffer, (uint32_t)len);
}

#ifndef PLATFORM_NO_XIP
int stub_miniz_deflate_raw(const volatile uint8_t* src, uint32_t len, uint8_t level,
	miniz_put_buffer_fn put_buffer, void* put_ctx)
{
	if(!put_buffer) return 0;
	if(level < 1U || level > 10U) level = 5U;
	miniz_output_t output = { put_buffer, put_ctx };
	int flags = (int)tdefl_create_comp_flags_from_zip_params(level, -15, MZ_DEFAULT_STRATEGY);
#if PLATFORM_RDA5981 || PLATFORM_ECR6600 || PLATFORM_TR6260
	deflator.m_next = m_next;
#endif
	if(tdefl_init(&deflator, deflate_output, &output, flags) != TDEFL_STATUS_OKAY) return 0;
	return tdefl_compress_buffer(&deflator, (const void*)(uintptr_t)src, len,
		TDEFL_FINISH) == TDEFL_STATUS_DONE;
}
#else
#define OUT_SIZE (BUF_SIZE / 2)
int stub_miniz_deflate_raw(const volatile uint8_t* src, uint32_t len, uint8_t level,
	miniz_put_buffer_fn put_buffer, void* put_ctx)
{
	if(!put_buffer) return 0;
	if(level < 1U || level > 10U) level = 5U;

	int flags = (int)tdefl_create_comp_flags_from_zip_params(level, -15, MZ_DEFAULT_STRATEGY);

	if(tdefl_init(&deflator, NULL, NULL, flags) != TDEFL_STATUS_OKAY) return 0;

	uint32_t flash_addr = (uint32_t)(uintptr_t)src;
	uint32_t remaining = len;
	uint8_t* out_buf = ((uint8_t*)&cmd_buf) + OUT_SIZE;

	while(remaining)
	{
		uint32_t input_len = remaining;

		if(input_len > OUT_SIZE)
			input_len = OUT_SIZE;

		stub_flash_read(cmd_buf, flash_addr, input_len);
		flash_addr += input_len;
		remaining -= input_len;

		uint32_t input_pos = 0U;

		while(input_pos < input_len)
		{
			size_t input_bytes = input_len - input_pos;
			size_t output_bytes = OUT_SIZE;
			tdefl_flush flush = remaining ? TDEFL_NO_FLUSH : TDEFL_FINISH;
			tdefl_status status = tdefl_compress(&deflator, cmd_buf + input_pos, &input_bytes, out_buf, &output_bytes, flush);

			if(output_bytes && !put_buffer(put_ctx, out_buf, (uint32_t)output_bytes))
			{
				return 0;
			}

			input_pos += (uint32_t)input_bytes;

			if(status == TDEFL_STATUS_DONE)
			{
				return remaining == 0U && input_pos == input_len;
			}

			if(status != TDEFL_STATUS_OKAY) return 0;
			if(!input_bytes && !output_bytes) return 0;
		}
	}

	if(len == 0U)
	{
		size_t input_bytes = 0U;
		size_t output_bytes = OUT_SIZE;
		tdefl_status status = tdefl_compress(&deflator, NULL, &input_bytes, out_buf, &output_bytes, TDEFL_FINISH);
		if(output_bytes && !put_buffer(put_ctx, out_buf, (uint32_t)output_bytes)) return 0;
		return status == TDEFL_STATUS_DONE;
	}

	return 0;
}
#endif
#endif

#ifndef NO_MINIZ_DECOMPRESSION
static int write_inflate_output(miniz_put_buffer_fn put_buffer, void* put_ctx,
	const uint8_t* buffer, uint32_t len,
	uint32_t* total, uint32_t expected_len)
{
	if(len > expected_len - *total) return 0;
	if(len && !put_buffer(put_ctx, buffer, len)) return 0;
	*total += len;
	return 1;
}

int stub_miniz_inflate_raw(miniz_get_buffer_fn get_buffer, void* get_ctx,
	miniz_put_buffer_fn put_buffer, void* put_ctx,
	uint32_t expected_len)
{
	if(!get_buffer || !put_buffer) return 0;
	tinfl_init(&inflator);
	uint32_t total = 0U;
	uint32_t dictionary_pos = 0U;

	for(;;)
	{
		uint32_t input_len = get_buffer(get_ctx, cmd_buf, sizeof(cmd_buf));
		if(!input_len) return 0;

		uint32_t input_pos = 0U;
		while(input_pos < input_len)
		{
			size_t input_bytes = input_len - input_pos;
			size_t output_bytes = TDEFL_LZ_DICT_SIZE - dictionary_pos;
			tinfl_status status = tinfl_decompress(&inflator,
				cmd_buf + input_pos, &input_bytes,
#ifdef NO_MINIZ_COMPRESSION
				inflate_dictionary, inflate_dictionary
#else
				deflator.m_dict, deflator.m_dict
#endif
				+ dictionary_pos, &output_bytes,
				TINFL_FLAG_HAS_MORE_INPUT);
			input_pos += (uint32_t)input_bytes;
			if(!write_inflate_output(put_buffer, put_ctx,
#ifdef NO_MINIZ_COMPRESSION
				inflate_dictionary
#else
				deflator.m_dict
#endif
				+ dictionary_pos, (uint32_t)output_bytes,
				&total, expected_len)) return 0;
			dictionary_pos += (uint32_t)output_bytes;
			if(dictionary_pos == TDEFL_LZ_DICT_SIZE) dictionary_pos = 0U;

			if(status == TINFL_STATUS_DONE) return total == expected_len;
			if(status < TINFL_STATUS_DONE) return 0;
			if(status == TINFL_STATUS_NEEDS_MORE_INPUT) break;
			if(!input_bytes && !output_bytes) return 0;
		}
	}
}
#endif
