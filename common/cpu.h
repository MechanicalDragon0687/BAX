#pragma once
#include <common.h>
#include <arm.h>

#define ENTER_CRITICAL(x) do { (x) = cpu_DisableIRQ(); } while(0)
#define LEAVE_CRITICAL(x) do { cpu_EnableIRQ((x)); } while(0)

static inline u32 cpu_ReadCPSR(void)
{
    u32 cpsr;
    asm("mrs %0, cpsr\n\t":"=r"(cpsr));
    return cpsr;
}

static inline void cpu_WriteCPSR(u32 cpsr)
{
    asm("msr cpsr_c, %0\n\t"::"r"(cpsr));
    return;
}

static inline u32 cpu_ReadCR(void)
{
    u32 cr;
    asm("mrc p15, 0, %0, c1, c0, 0\n\t":"=r"(cr));
    return cr;
}

static inline void cpu_WriteCR(u32 cr)
{
    asm("mcr p15, 0, %0, c1, c0, 0\n\t"::"r"(cr));
    return;
}

static inline u32 cpu_DisableIRQ(void)
{
    u32 cpsr = cpu_ReadCPSR();
    #ifdef ARM9
    cpu_WriteCPSR(cpsr | SR_IRQ_BIT | SR_FIQ_BIT);
    #else
    asm("cpsid if\n\t");
    #endif
    return cpsr & (SR_IRQ_BIT | SR_FIQ_BIT);
}

static inline void cpu_EnableIRQ(u32 ss)
{
    #ifdef ARM9
    u32 cpsr = cpu_ReadCPSR() & ~(SR_IRQ_BIT | SR_FIQ_BIT);
    cpu_WriteCPSR(cpsr | ss);
    #else
    if (!(ss & SR_IRQ_BIT)) {
        asm("cpsie if\n\t");
    }
    #endif
    return;
}

static inline void cpu_InvalidateIC(void)
{
    asm("mcr p15, 0, %0, c7, c5, 0\n\t"::"r"(0));
    return;
}

static inline void cpu_InvalidateICRange(u32 start, u32 end)
{
    start &= ~0x1F;
    while(start < end) {
        asm("mcr p15, 0, %0, c7, c5, 1"::"r"(start));
        start += 0x20;
    }
    return;
}

static inline void cpu_InvalidateDC(void)
{
    asm("mcr p15, 0, %0, c7, c6, 0\n\t"::"r"(0));
    return;
}

static inline void cpu_InvalidateDCRange(u32 start, u32 end)
{
    start &= ~0x1F;
    while(start < end) {
        asm("mcr p15, 0, %0, c7, c6, 1"::"r"(start));
        start += 0x20;
    }
    return;
}

static inline void cpu_WritebackDC(void)
{
    #ifdef ARM9
    u32 seg=0,ind;
    do {
        ind=0;
        do {
            asm("mcr p15, 0, %0, c7, c10, 2\n\t"::"r"(seg|ind));
            ind+=0x20;
        } while(ind<0x400);
        seg+=0x40000000;
    } while(seg);
    asm("mcr p15, 0, %0, c7, c10, 4\n\t"::"r"(0));
    #else
    asm("mcr p15, 0, %0, c7, c10, 0\n\t"::"r"(0));
    #endif
    return;
}

static inline void cpu_WritebackDCRange(u32 start, u32 end)
{
    start &= ~0x1F;
    while(start < end) {
        asm("mcr p15, 0, %0, c7, c10, 1"::"r"(start));
        start += 0x20;
    }
    #ifdef ARM9
    asm("mcr p15, 0, %0, c7, c10, 4\n\t"::"r"(0));
    #endif
    return;
}

#ifdef ARM11
static inline void cpu_DSB(void)
{
    asm("mcr p15, 0, %0, c7, c10, 4\n\t"::"r"(0));
    return;
}

static inline void cpu_DMB(void)
{
    asm("mcr p15, 0, %0, c7, c10, 5\n\t"::"r"(0));
    return;
}
#endif

static inline void cpu_WBInvDCRange(u32 start, u32 end)
{
    cpu_WritebackDCRange(start,end);
    cpu_InvalidateICRange(start,end);
}

static inline void cpu_InvalidateAll(void)
{
    #ifdef ARM9
    cpu_InvalidateDC();
    cpu_InvalidateIC();
    #else
    asm("mcr p15, 0, %0, c7, c7, 0\n\t"::"r"(0));
    #endif
    return;
}

static inline void wfi(void)
{
    #ifdef ARM9
    asm("mcr p15, 0, %0, c7, c0, 4\n\t"::"r"(0));
    #else
    asm("wfi\n\t");
    #endif
    return;
}
