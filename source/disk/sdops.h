#pragma once

#include <stdint.h>
#include <stddef.h>

#include <disk/fatfs/ff.h>

bool file_exists(const char *path);
size_t file_size(const char *path);
size_t file_read_offset(uint8_t *dest, const char *path, const size_t offset, const size_t max_size);
size_t file_read(uint8_t *dest, const char *path, const size_t max_size);
uint32_t find_files(const char *base_path, const char *pattern, uint32_t max, char out[][_MAX_LFN + 1]);
