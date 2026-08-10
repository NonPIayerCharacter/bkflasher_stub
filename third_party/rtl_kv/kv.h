#include <stdint.h>
#define MAX_KEY_LENGTH 256
int rt_kv_init(void);
int32_t rt_kv_set(const char *key, const void *val, int32_t len);
int32_t rt_kv_get(const char *key, void *buffer, int32_t len);
int32_t rt_kv_size(const char *key);
