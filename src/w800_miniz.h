#ifndef W800_MINIZ_H
#define W800_MINIZ_H

#include <stdint.h>

typedef uint32_t (*w800_miniz_get_buffer_fn)(void *ctx, uint8_t *buffer, uint32_t capacity);
typedef int (*w800_miniz_put_buffer_fn)(void *ctx, const uint8_t *buffer, uint32_t len);

int w800_miniz_inflate_raw(w800_miniz_get_buffer_fn get_buffer, void *get_ctx,
                           w800_miniz_put_buffer_fn put_buffer, void *put_ctx,
                           uint32_t expected_len);

int w800_miniz_deflate_raw(const volatile uint8_t *src, uint32_t len, uint8_t level,
                           w800_miniz_put_buffer_fn put_buffer, void *put_ctx);

#endif
