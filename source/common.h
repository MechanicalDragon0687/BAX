#pragma once

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define DTCM   __attribute__((section(".dtcm")))
#define ITCM   __attribute__((section(".itcm")))
#define UNUSED __attribute__((unused))

#define asm          __asm__ volatile
#define abort()      asm("bkpt 0xFACC\n\t")
/* breakpoint instruction, triggers a prefetch abort */

#define BIT(x)       (1 << (x))
#define SWAP(a,b)    do {typeof (a) __c; __c = (a); (a) = (b); (b) = __c;} while(0)

static inline int read_rand(void)
{
    return *(volatile int*)(0x10011000);
}

static inline uint8_t read_bootenv(void)
{
    return *(volatile uint8_t*)(0x10010000);
}

#define MAX_ANIMATIONS (32)

#define min(a,b)     ((a)<(b)?(a):(b))
#define max(a,b)     ((a)>(b)?(a):(b))

#define BASE_PATH         "0:/bax"
#define CHAINLOAD_PATH    BASE_PATH "/boot.bin"

char *msprintf(char *out, const char *fmt, ...);
void fastcpy(void *dest, void *source, size_t n);
