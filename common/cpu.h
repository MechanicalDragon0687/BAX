#ifndef CPU_H
#define CPU_H

#include <common.h>
#include <asm.h>

#ifndef __ASSEMBLER__

typedef u32 CritStat;

static inline u32 CPU_GetCPSR(void) {
    u32 cpsr;
    asmv("mrs %0, cpsr\n\t"
        :"=r"(cpsr));
    return cpsr;
}

static inline void CPU_SetCPSR(u32 cpsr) {
    asmv("msr cpsr_c, %0\n\t"
        ::"r"(cpsr));
}

static inline CritStat CPU_EnterCritical(void) {
    CritStat stat = CPU_GetCPSR();
    CPU_SetCPSR(stat | SR_NOINT);
    return stat & SR_NOINT;
}

static inline void CPU_LeaveCritical(CritStat stat) {
    CPU_SetCPSR(stat | (CPU_GetCPSR() & ~SR_NOINT));
}

static inline u32 CPU_CoreID(void) {
    u32 ret;

    #ifdef ARM9
    ret = 0;
    #else
    asmv("mrc p15, 0, %0, c0, c0, 5\n\t"
        :"=r"(ret));
    #endif

    return ret;
}

static inline void CPU_WFI(void) {
    #ifdef ARM9
    asmv("mcr p15, 0, %0, c7, c0, 4\n\t"
        ::"r"(0));
    #else
    asmv("wfi\n\t");
    #endif // ARM9
}

#ifdef ARM11
static inline void CPU_WFE(void) {
    asmv("wfe\n\t");
}
#endif // ARM11

#endif // __ASSEMBLER__

#endif // CPU_H
