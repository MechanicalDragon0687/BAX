#pragma once

#ifndef ARM9
#ifndef ARM11
#error "Undefined processor!"
#endif
#endif

#undef CORE_COUNT
#ifdef ARM9
#define CORE_COUNT (1)
#else
#define CORE_COUNT (4)
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

typedef volatile u8  vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;

typedef volatile s8  vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile s64 vs64;

// Forces failure at compile time if (cond == false)
#define COMPILE_ASSERT(cond) ((void)sizeof(char[1 - 2*!(cond)]))

#define ALIGNV(x)    __attribute__((aligned((x))))
#define UNUSED       __attribute__((unused))
#define NORETURN     __attribute__((noreturn))
#define NOINLINE     __attribute__((noinline))
#define PACKED       __attribute__((packed))

// Extend char to integer
#define EXTENDET(x)  ((x) << 24 | (x) << 16 | (x) << 8 | (x))

// Extend short to integer
#define EXTENDST(x)  ((x) << 16 | (x))

#define BIT(x)       (1<<(x))
#define max(a,b)     ((a)>(b)?(a):(b))
#define min(a,b)     ((a)<(b)?(a):(b))
