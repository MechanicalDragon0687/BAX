#pragma once

#include <common.h>
#include <asm.h>

static inline u32 _read_CPSR(void)
{
    u32 cpsr;
    __asm__ __volatile__ (
        "mrs %0, cpsr\n\t"
        : "=r"(cpsr)
    );
    return cpsr;
}

static inline void _write_CPSR_c(u32 cpsr)
{
    __asm__ __volatile__ (
        "msr cpsr_c, %0\n\t"
        : : "r"(cpsr)
    );
    return;
}

static inline void _disable_irqs(void)
{
    u32 tmp;
    __asm__ __volatile__ (
        #ifdef ARM9
        "mrs %0, cpsr\n\t"
        "orr %0, #0xC0\n\t"
        "msr cpsr_c, %0\n\t"
        : "=r"(tmp) : : "memory"
        #else
        "cpsid if\n\t" : : : "memory"
        #endif
    );
    return;
}

static inline void _enable_irqs(void)
{
    u32 tmp;
    __asm__ __volatile__ (
        #ifdef ARM9
        "mrs %0, cpsr\n\t"
        "bic %0, #0xC0\n\t"
        "msr cpsr_c, %0\n\t"
        : "=r"(tmp) : : "memory"
        #else
        "cpsie if\n\t" : : : "memory"
        #endif
    );
    return;
}

static inline u32 _enter_critical(void)
{
    u32 stat = _read_CPSR();
    _write_CPSR_c(stat | SR_I);
    return stat & SR_I;
}

static inline void _leave_critical(u32 stat)
{
    _write_CPSR_c(stat | (_read_CPSR() & ~SR_I));
    return;
}

static inline void _wfi(void)
{
    __asm__ __volatile__ (
        #ifdef ARM9
        "mcr p15, 0, %0, c7, c0, 4\n\t"
        : : "r"(0)
        #else
        "wfi\n\t"
        #endif
    );
    return;
}

static inline void _write_CR(u32 cr)
{
    __asm__ __volatile__ (
        "mcr p15, 0, %0, c1, c0, 0\n\t"
        : : "r"(cr) : "memory"
    );
    return;
}

static inline u32 _read_CR(void)
{
    u32 cr;
    __asm__ __volatile__ (
        "mrc p15, 0, %0, c1, c0, 0\n\t"
        : "=r"(cr)
    );
    return cr;
}

static inline u32 _coreID(void)
{
    u32 id;
    #ifdef ARM9
    id = 0;
    #else
    __asm__ __volatile__ (
        "mrc p15, 0, %0, c0, c0, 5\n\t"
        : "=r"(id)
    );
    id &= 3;
    #endif
    return id;
}

#ifdef ARM11
static inline void _wfe(void)
{
    __asm__ __volatile__ (
        "wfe\n\t"
    );
    return;
}

static inline void _write_ACR(u32 acr)
{
    __asm__ __volatile__ (
        "mcr p15, 0, %0, c1, c0, 1\n\t"
        : : "r"(acr) : "memory"
    );
    return;
}

static inline u32 _read_ACR(void)
{
    u32 acr;
    __asm__ __volatile__ (
        "mrc p15, 0, %0, c1, c0, 1\n\t"
        : "=r"(acr)
    );
    return acr;
}
#endif
