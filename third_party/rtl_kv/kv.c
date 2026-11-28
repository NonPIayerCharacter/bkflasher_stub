#include "kv.h"
#include "../littlefs/lfs.h"

extern lfs_t g_lfs;
int DiagSnPrintf(char* buf, size_t size, const char* fmt, ...);

/* The command loop serializes all KV operations. */
static char path[MAX_KEY_LENGTH + 2];
static lfs_file_t file;
static uint8_t file_cache[RTL_KV_CACHE_SIZE] __attribute__((aligned(8)));
static const struct lfs_file_config file_config = { .buffer = file_cache };

static int make_path(const char* key)
{
	if(strlen(key) > MAX_KEY_LENGTH - 3)
		return 0;
	DiagSnPrintf(path, sizeof(path), "KV/%s", key);
	return 1;
}

int rt_kv_init(void)
{
	int ret = lfs_mkdir(&g_lfs, "KV");
	return ret == LFS_ERR_EXIST ? 0 : ret;
}

int32_t rt_kv_set(const char* key, const void* val, int32_t len)
{
	if(!make_path(key)) return -1;
	int ret = lfs_file_opencfg(&g_lfs, &file, path, LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC, &file_config);
	if(ret < 0) return ret;
	lfs_file_write(&g_lfs, &file, val, len);
	lfs_file_close(&g_lfs, &file);
	return ret;
}

int32_t rt_kv_get(const char* key, void* buffer, int32_t len)
{
	if(!make_path(key)) return -1;
	int ret = lfs_file_opencfg(&g_lfs, &file, path, LFS_O_RDONLY, &file_config);
	if(ret < 0) return ret;
	lfs_file_read(&g_lfs, &file, buffer, len);
	lfs_file_close(&g_lfs, &file);
	return ret;
}

int32_t rt_kv_size(const char* key)
{
	if(!make_path(key)) return -1;
	struct lfs_info info;
	int ret = lfs_stat(&g_lfs, path, &info);
	if(ret >= 0)
		ret = info.size;
	return ret;
}
