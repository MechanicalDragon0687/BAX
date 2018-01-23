#pragma once

#include <common.h>

#define MAX_PATH (255)

int fs_init(void);

int fs_list(const char *dirpath, const char *pattern, char **paths, int max);
size_t fs_read(const char *path, void *dest, size_t max);
size_t fs_size(const char *path);
