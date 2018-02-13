/**
Copyright 2018 Wolfvak

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*/

#ifndef ENDIAN_H
#define ENDIAN_H

#include <stddef.h>

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
