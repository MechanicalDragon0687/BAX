#pragma once

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
