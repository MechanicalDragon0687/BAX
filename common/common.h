#pragma once

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef ARM9
#ifndef ARM11
#error "Undefined target"
#endif
#endif

#define UNUSED __attribute__((unused))

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t   u8;

typedef volatile u64 vu64;
typedef volatile u32 vu32;
typedef volatile u16 vu16;
typedef volatile u8   vu8;

#define asm __asm__ volatile

#define BIT(x)       (1<<(x))
#define CLAMP(x,a,b) ((x)<(a)?(a):((x)>(b))?(b):(x))
#define MIN(a,b)     (((a)<(b))?(a):(b))
#define MAX(a,b)     (((a)>(b))?(a):(b))
#define RANGE(x,a,b) ((x)>=(a)&&(x)<=(b))

#define BASE_PATH     "0:/bax"
#define SDMC_PATH     "sdmc:/bax"
#define ANIM_SUBPATH  "/test.bax"
#define FIRM_SUBPATH  "/boot.firm"

#define ANIM_BUFFER   ((void*)(0x20000000))
#define FIRM_BUFFER   ((void*)(0x22000000))

#define MPCORE_ENTRY  (0x1FFFFFFC)
#define FRAMEBUFFERS  (0x27FFFC00)

#define ANIM_MAXSIZE  (0x2000000)
#define FIRM_MAXSIZE  (0x400000)

#ifdef ARM9
#define REG_PRNG    ((vu32*)(0x10011000))
#define REG_BOOTENV ((vu32*)(0x10010000))
#endif
