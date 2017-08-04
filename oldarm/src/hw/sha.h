#pragma once
#include <common.h>

#define SHA_BASE (0x1000A000)

#define SHA_CNT    ((vu32*)(SHA_BASE + 0x00))
#define SHA_BLKCNT ((vu32*)(SHA_BASE + 0x04))
#define SHA_HASH   ((vu32*)(SHA_BASE + 0x40))
#define SHA_FIFO   ((vu32*)(SHA_BASE + 0x80))

#define SHA_START  (BIT(0))
#define SHA_FINAL  (BIT(1))
#define SHA_ENDIAN (BIT(3))

#define SHA256 (0<<4)
#define SHA224 (1<<4)
#define SHA1   (2<<4)

void sha(u32 *hash, const u32 *data, u32 length, u32 mode);
