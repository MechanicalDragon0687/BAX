#pragma once

#include <common.h>
#include <arm/arm.h>

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

/* Invalidate the entire Instruction Cache */
static inline void invalidate_icache_all(void)
{
    asm("mcr p15, 0, %0, c7, c5, 0\n\t"::"r"(0));
    return;
}

/* Invalidate the entire Data Cache */
static inline void invalidate_dcache_all(void)
{
    asm("mcr p15, 0, %0, c7, c6, 0\n\t"::"r"(0));
    return;
}

static inline void writeback_dcache_range(uint32_t start, uint32_t end)
{
    start &= ~0x1F;
    while(start < end) {
        asm("mcr p15, 0, %0, c7, c10, 1"::"r"(start));
        start += 0x20;
    }
    return;
}

static inline void invalidate_icache_range(uint32_t start, uint32_t end)
{
    start &= ~0x1F;
    while(start < end) {
        asm("mcr p15, 0, %0, c7, c5, 1"::"r"(start));
        start += 0x20;
    }
    return;
}

static inline void invalidate_dcache_range(uint32_t start, uint32_t end)
{
    start &= ~0x1F;
    while(start < end) {
        asm("mcr p15, 0, %0, c7, c6, 1"::"r"(start));
        start += 0x20;
    }
    return;
}

static inline void writeback_invalidate_dcache_range(uint32_t start, uint32_t end)
{
    start &= ~0x1F;
    while(start < end) {
        asm("mcr p15, 0, %0, c7, c14, 1"::"r"(start));
        start += 0x20;
    }
    return;
}

static inline void drain_write_buffer(void)
{
    asm("mcr p15, 0, %0, c7, c10, 4\n\t"::"r"(0));
    return;
}

/* Wait For Interrupt (or debug signal) */
static inline void wfi(void)
{
    asm("mcr p15, 0, %0, c7, c0, 4\n\t"
        ::"r"(0));
    return;
}
