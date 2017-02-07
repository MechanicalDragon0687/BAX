#pragma once

#include <stdint.h>
#include <stddef.h>

#include <disk/fatfs/ff.h>

bool file_exists(const char *path);
u32 file_size(const char *path);
u32 file_read_offset(void *dest, const char *path, const u32 offset, const u32 max_size);
u32 file_read(void *dest, const char *path, const u32 max_size);
u32 find_files(const char *base_path, const char *pattern, const u32 max, char out[][_MAX_LFN + 1]);
