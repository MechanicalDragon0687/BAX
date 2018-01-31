#pragma once

#include <common.h>
#include <asm.h>

#ifndef __ASSEMBLER__
static inline u32 _get_CPSR(void)
{
    u32 cpsr;
    __asm__ __volatile__ ("mrs %[cpsr], cpsr\n\t":[cpsr]"=r"(cpsr));
    return cpsr;
}

static inline void _set_CPSR(u32 cpsr)
{
    __asm__ __volatile__ ("msr cpsr_c, %[cpsr]\n\t"::[cpsr]"r"(cpsr));
}

static inline u32 _enter_critical(void)
{
    u32 stat = _get_CPSR();
    _set_CPSR(stat | SR_NOIRQ);
    return stat & SR_NOIRQ;
}

static inline void _leave_critical(u32 stat)
{
    _set_CPSR(stat | (_get_CPSR() & ~SR_NOIRQ));
    return;
}


static inline void _wfi(void)
{
    __asm__ __volatile__ (
        #ifdef ARM9
        "mcr p15, 0, %0, c7, c0, 4\n\t"::"r"(0)
        #else
        "wfi\n\t"
        #endif
    );
}

static inline u32 _coreID(void)
{
    u32 id;
    #ifdef ARM9
    id = 0;
    #else
    __asm__ __volatile__ ("mrc p15, 0, %0, c0, c0, 5\n\t":"=r"(id));
    id &= 3;
    #endif
    return id;
}

#ifdef ARM11
static inline void _wfe(void)
{
    __asm__ __volatile__ ("wfe\n\t");
}
#endif
#endif
