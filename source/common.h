#pragma once

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define DTCM   __attribute__((section(".dtcm")))
#define ITCM   __attribute__((section(".itcm")))
#define UNUSED __attribute__((unused))

#define asm     __asm__ volatile
#define abort() asm("bkpt 0x7FFF\n\t")

#define rand()    (*(volatile int*)(0x10011000))
#define bootenv() (*(volatile uint32_t*)(0x10010000))

#define BIT(x)       (1<<(x))
#define CLAMP(x,a,b) ((x)<(a)?(a):((x)>(b))?(b):(x))
//#define RANGE(x,a,b) ((x)>=(a)&&(x)<=(b))

#define MIN(a,b)     (((a)<(b))?(a):(b))
#define MAX(a,b)     (((a)>(b))?(a):(b))

#define assert(x)   do{if(!(x)){abort();}}while(0)
#define SWAP(a,b)   do{__typeof__ (a) __S;__S=(a);(a)=(b);(b)=__S;}while(0)

static bool RANGE(uint32_t x, uint32_t a, uint32_t b)
{
    return (x >= a && x <= b);
}

static inline bool addr_is_cached(uint32_t addr)
{
    if (RANGE(addr, 0x20000000, 0x30000000) || /* FCRAM */
        RANGE(addr, 0x08000000, 0x08180000) || /* ARM9 RAM */
        RANGE(addr, 0x18000000, 0x18600000)) { /* VRAM */
        return true;
    } else {
        return false;
    }
}

static inline bool addr_is_exec(uint32_t addr)
{
    if (RANGE(addr, 0x00000000, 0x08000000) || /* ITCM */
        RANGE(addr, 0x20000000, 0x30000000)) { /* FCRAM */
        return true;
    } else {
        return false;
    }
}

static inline bool addr_is_data(uint32_t addr)
{
    if (RANGE(addr, 0x08000000, 0x08180000) || /* ARM9 RAM */
        RANGE(addr, 0x18000000, 0x18600000) || /* VRAM */
        RANGE(addr, 0x1FF00000, 0x20000000) || /* DSP / AXI WRAM */
        RANGE(addr, 0x20000000, 0x30000000) || /* FCRAM */
        RANGE(addr, 0xFFF00000, 0xFFF04000) || /* DTCM */
        RANGE(addr, 0xFFFF0000, 0xFFFF8000)) { /* BootROM */
        return true;
    } else {
        return false;
    }
}

#define BASE_PATH         "0:/bax"
#define CHAINLOAD_PATH    BASE_PATH "/boot.firm"

char *msprintf(char *out, const char *fmt, ...);
void fastcpy(void *dest, void *source, size_t n);
