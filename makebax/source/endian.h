/**
Copyright 2018 Wolfvak
For more information, read LICENSE.
*/


#ifndef ENDIAN_H
#define ENDIAN_H

#include <stdint.h>

static inline uint16_t mem_le16(uint8_t *p)
{
    uint16_t ret;
    ret  = static_cast<uint16_t> (p[1]) << 8;
    ret |= static_cast<uint16_t> (p[0]);
    return ret;
}

static inline uint16_t mem_be16(uint8_t *p)
{
    uint16_t ret;
    ret  = static_cast<uint16_t> (p[0]) << 8;
    ret |= static_cast<uint16_t> (p[1]);
    return ret;
}

static inline uint32_t mem_le32(uint8_t *p)
{
    uint32_t ret;
    ret  = static_cast<uint32_t> (mem_le16(p + 2)) << 16;
    ret |= static_cast<uint32_t> (mem_le16(p + 0));
    return ret;
}

static inline uint32_t mem_be32(uint8_t *p)
{
    uint32_t ret;
    ret  = static_cast<uint32_t> (mem_be16(p + 0)) << 16;
    ret |= static_cast<uint32_t> (mem_be16(p + 2));
    return ret;
}

static inline uint64_t mem_le64(uint8_t *p)
{
    uint64_t ret;
    ret  = static_cast<uint64_t> (mem_le32(p + 4)) << 32;
    ret |= static_cast<uint64_t> (mem_le32(p + 0));
    return ret;
}

static inline uint64_t mem_be64(uint8_t *p)
{
    uint64_t ret;
    ret  = static_cast<uint64_t> (mem_be32(p + 0)) << 32;
    ret |= static_cast<uint64_t> (mem_be32(p + 4));
    return ret;
}


#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define le16(x) ((x))
#define le32(x) ((x))
#define le64(x) ((x))

#define be16(x) (__builtin_bswap16(x))
#define be32(x) (__builtin_bswap32(x))
#define be64(x) (__builtin_bswap64(x))

#else

#define le16(x) (__builtin_bswap16(x))
#define le32(x) (__builtin_bswap32(x))
#define le64(x) (__builtin_bswap64(x))

#define be16(x) ((x))
#define be32(x) ((x))
#define be64(x) ((x))
#endif

#endif
