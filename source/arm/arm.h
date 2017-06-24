#pragma once

#include <common.h>

#define USR_MODE (0x10)
#define FIQ_MODE (0x11)
#define IRQ_MODE (0x12)
#define SVC_MODE (0x13)
#define ABT_MODE (0x17)
#define UND_MODE (0x1B)
#define SYS_MODE (0x1F)
#define ANY_MODE (0x1F)

#define TMB_BIT (BIT(5))
#define FIQ_BIT (BIT(6))
#define IRQ_BIT (BIT(7))

#define ENABLE_MPU    (BIT(0))
#define ENABLE_DCACHE (BIT(2))
#define ENABLE_ICACHE (BIT(12))
#define ENABLE_DTCM   (BIT(16))
#define ENABLE_ITCM   (BIT(18))
#define ALT_VECTORS   (BIT(13))
#define CACHE_RROBIN  (BIT(14))

static inline uint32_t read_cr1(void)
{
    uint32_t cr;
    asm("mrc p15, 0, %0, c1, c0, 0\n\t":"=r"(cr));
    return cr;
}

static inline void write_cr1(uint32_t cr)
{
    asm("mcr p15, 0, %0, c1, c0, 0\n\t"::"r"(cr));
    return;
}

static inline uint32_t read_cpsr(void)
{
    uint32_t cpsr;
    asm("mrs %0, cpsr\n\t":"=r"(cpsr));
    return cpsr;
}

static inline void write_cpsr(uint32_t cpsr)
{
    asm("msr cpsr, %0\n\t"::"r"(cpsr));
    return;
}

static inline void flush_icache(void)
{
    asm("mcr p15, 0, %0, c7, c5, 0\n\t"::"r"(0));
    return;
}

static inline void flush_dcache(void)
{
    asm("mcr p15, 0, %0, c7, c6, 0\n\t"::"r"(0));
    return;
}

static inline void flush_dcache_range(void *start, void *end)
{
    while(start < end)
    {
        asm("mcr p15, 0, %0, c7, c14, 1"::"r"(start));
        start += 0x20;
    }
    return;
}

static inline void dwb(void)
{
    asm("mcr p15, 0, %0, c7, c10, 4\n\t"::"r"(0));
    return;
}

static inline void wfi(void)
{
    asm("mcr p15, 0, %0, c7, c0, 4\n\t"
        ::"r"(0));
    return;
}

void flush_all(void);
void chainload(void *payload, void *entry, size_t len);
