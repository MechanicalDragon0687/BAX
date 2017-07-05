#pragma once

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

extern int _itcm_lma, _itcm_len, _dtcm_lma, _dtcm_len, _sbss, _ebss;
extern int _itcm_loc, _dtcm_loc;

#define DTCM   __attribute__((section(".dtcm")))
#define ITCM   __attribute__((section(".itcm")))
#define UNUSED __attribute__((unused))

#define asm          __asm__ volatile
#define abort()      asm("bkpt\n\t")

#define min(a,b)     ((a)<(b)?(a):(b))
#define max(a,b)     ((a)>(b)?(a):(b))

#define BIT(x)       (1 << (x))
#define SWAP(a,b)    do {typeof (a) __swp_tmp_var; __swp_tmp_var = (a); (a) = (b); (b) = __swp_tmp_var;} while(0)
#define ARR_COUNT(x) (sizeof((x))/sizeof(*(x)))
#define CLAMP(x,a,b) ((x)<(a)?(a):((x)>(b))?(b):(x))

static inline __attribute__((noreturn)) void abort_code(uint32_t code)
{
    asm("mov r0, %0\n\t" /* <- isn't really necessary but just in case... */
        "bkpt\n\t"::"r"(code));
    while(1);
}

static inline int read_rand(void)
{
    return *(volatile int*)(0x10011000);
}

static inline bool addr_is_cached(uint32_t addr)
{
    if ((addr >= 0x20000000 && addr <= 0x30000000) ||
        (addr >= 0x08000000 && addr <= 0x08100000)) {
        return true;
    } else {
        return false;
    }
}

#define MAX_ANIMATIONS (32)

#define BASE_PATH         "0:/bax"
#define CHAINLOAD_PATH    BASE_PATH "/boot.bin"

char *msprintf(char *out, const char *fmt, ...);
void fastcpy(void *dest, void *source, size_t n);
