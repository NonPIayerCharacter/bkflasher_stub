#ifndef STUB_MINIZ_H
#define STUB_MINIZ_H

#include <stdint.h>

typedef uint32_t(*miniz_get_buffer_fn)(void* ctx, uint8_t* buffer, uint32_t capacity);
typedef int (*miniz_put_buffer_fn)(void* ctx, const uint8_t* buffer, uint32_t len);

int stub_miniz_inflate_raw(miniz_get_buffer_fn get_buffer, void* get_ctx,
	miniz_put_buffer_fn put_buffer, void* put_ctx,
	uint32_t expected_len);

int stub_miniz_deflate_raw(const volatile uint8_t* src, uint32_t len, uint8_t level,
	miniz_put_buffer_fn put_buffer, void* put_ctx);

#endif
