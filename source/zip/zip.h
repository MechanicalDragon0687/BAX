#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <common.h>

#define ZIP_MAGIC 0x04034B50 // PK \03 \04

typedef struct
{
    u32 magic;
    u16 version;
    u16 flags;
    u16 comp;
    u16 time;
    u16 date;
    u32 crc32;
    u32 size;
    u32 raw_size;
    u16 name_len;
    u16 meta_len;
} __attribute__((packed)) zipf_entry;
// ZIP entry as it appears on a regular ZIP file

size_t zip_extract_file(const char *filename, void *dest, size_t max_size, u8 *zip_data, size_t zip_len);
