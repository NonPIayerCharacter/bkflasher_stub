#include "kv.h"
#include "../littlefs/lfs.h"

extern lfs_t g_lfs;

#if PLATFORM_RTL8721DA || PLATFORM_RTL8720E
int DiagSnPrintf(char* buf, size_t size, const char* fmt, ...);
#define snprintf DiagSnPrintf
#endif

int rt_kv_init(void)
{
	int ret = -1;
	char* path = NULL;

	if((path = malloc(MAX_KEY_LENGTH + 1)) == NULL)
	{
		goto exit;
	}
	ret = lfs_mkdir(&g_lfs, "KV");
	if(ret == LFS_ERR_EXIST)
	{
		ret = 0;
	}

exit:
	if(path)
	{
		free(path);
	}
	return ret;
}

int32_t rt_kv_set(const char* key, const void* val, int32_t len)
{
	char* path = NULL;

	if((path = malloc(MAX_KEY_LENGTH + 2)) == NULL)
	{
		goto exit;
	}

	if(strlen(key) > MAX_KEY_LENGTH - 3)
	{
		goto exit;
	}

	snprintf(path, MAX_KEY_LENGTH + 2, "KV/%s", key);
	lfs_file_t* file = malloc(sizeof(lfs_file_t));
	if(file == NULL)
	{
		goto exit;
	}

	int ret = lfs_file_open(&g_lfs, file, path, LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC);
	if(ret < 0)
	{
		free(file);
		goto exit;
	}

	lfs_file_write(&g_lfs, file, val, len);
	lfs_file_close(&g_lfs, file);
	free(file);

exit:
	if(path)
	{
		free(path);
	}

	return ret;
}

int32_t rt_kv_get(const char* key, void* buffer, int32_t len)
{
	char* path = NULL;

	if((path = malloc(MAX_KEY_LENGTH + 2)) == NULL)
	{
		goto exit;
	}

	if(strlen(key) > MAX_KEY_LENGTH - 3)
	{
		goto exit;
	}

	snprintf(path, MAX_KEY_LENGTH + 2, "KV/%s", key);
	lfs_file_t* file = malloc(sizeof(lfs_file_t));
	if(file == NULL)
	{
		goto exit;
	}

	int ret = lfs_file_open(&g_lfs, file, path, LFS_O_RDONLY);
	if(ret < 0)
	{
		free(file);
		goto exit;
	}

	lfs_file_read(&g_lfs, file, buffer, len);
	lfs_file_close(&g_lfs, file);
	free(file);

exit:
	if(path)
	{
		free(path);
	}

	return ret;
}

int32_t rt_kv_size(const char* key)
{
	char* path = NULL;

	if((path = malloc(MAX_KEY_LENGTH + 2)) == NULL)
	{
		goto exit;
	}

	if(strlen(key) > MAX_KEY_LENGTH - 3)
	{
		goto exit;
	}

	snprintf(path, MAX_KEY_LENGTH + 2, "KV/%s", key);

	struct lfs_info info;
	int ret = lfs_stat(&g_lfs, path, &info);
	if(ret >= 0)
	{
		ret = info.size;
	}

exit:
	if(path)
	{
		free(path);
	}

	return ret;
}