#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ZIP_MAGIC 0x04034B50 // PK \03 \04

typedef struct
{
    uint32_t magic;
    uint16_t version;
    uint16_t flags;
    uint16_t comp;
    uint16_t time;
    uint16_t date;
    uint32_t crc32;
    uint32_t size;
    uint32_t raw_size;
    uint16_t name_len;
    uint16_t meta_len;
} __attribute__((packed)) zipf_entry;
// ZIP entry as it appears on a regular ZIP file

size_t zip_extract_file(const char *filename, uint8_t *dest, uint8_t *zip_data, const size_t max_size);
