#ifndef CACHE_H
#define CACHE_H

#include <common.h>


#ifdef ARM9

static inline void DrainBranchTarget(void) {}
static inline void DrainPrefetchBuffer(void) {}
static inline void DataMemoryBarrier(void) {}

#else

static inline void DrainBranchTarget(void) {
    asmv("mcr p15, 0, %0, c7, c5, 6\n\t"
        ::"r"(0):"memory");
}

static inline void DrainPrefetchBuffer(void) {
    asmv("mcr p15, 0, %0, c7, c5, 4\n\t"
        ::"r"(0):"memory");
}

static inline void DataMemoryBarrier(void) {
    asmv("mcr p15, 0, %0, c7, c10, 5\n\t"
        ::"r"(0):"memory");
}

#endif // ARM9


static inline void DataSyncBarrier(void) {
    asmv("mcr p15, 0, %0, c7, c10, 4\n\t"
        ::"r"(0):"memory");
}

static inline void CACHE_InvIC(void) {
    asmv("mcr p15, 0, %0, c7, c5, 0\n\t"
        ::"r"(0):"memory");

    DrainPrefetchBuffer();
    DrainBranchTarget();
    DataSyncBarrier();
}

static inline void CACHE_InvICRange(const void *base, u32 len) {
    u32 addr = (u32)base & ~0x1F;
    len >>= 5;

    do {
        asmv("mcr p15, 0, %0, c7, c5, 1\n\t"
            ::"r"(addr):"memory");

        addr += 0x20;
    } while(len--);

    DrainPrefetchBuffer();
    DrainBranchTarget();
    DataSyncBarrier();
}

static inline void CACHE_InvDC(void) {
    asmv("mcr p15, 0, %0, c7, c6, 0\n\t"
        ::"r"(0):"memory");
    DataSyncBarrier();
}

static inline void CACHE_InvDCRange(const void *base, u32 len) {
    u32 addr = (u32)base & ~0x1F;
    len >>= 5;

    do {
        asmv("mcr p15, 0, %0, c7, c6, 1"
            ::"r"(addr):"memory");
        addr += 0x20;
    } while(len--);

    DataSyncBarrier();
}

static inline void CACHE_WbDC(void) {
    #ifdef ARM9

    u32 seg = 0, ind;
    do {
        ind = 0;
        do {
            asmv("mcr p15, 0, %0, c7, c10, 2\n\t"
               ::"r"(seg | ind):"memory");

            ind += 0x20;
        } while(ind < 0x400);
        seg += 0x40000000;
    } while(seg != 0);

    #else

    asmv("mcr p15, 0, %0, c7, c10, 0\n\t"
        ::"r"(0):"memory");

    #endif // ARM9
    DataSyncBarrier();
}

static inline void CACHE_WbDCRange(const void *base, u32 len) {
    u32 addr = (u32)base & ~0x1F;
    len >>= 5;

    do {
        asmv("mcr p15, 0, %0, c7, c10, 1"
            ::"r"(addr):"memory");

        addr += 0x20;
    } while(len--);

    DataSyncBarrier();
}

static inline void CACHE_WbInvDC(void) {
    #ifdef ARM9

    u32 seg = 0, ind;
    do {
        ind = 0;
        do {
            asmv("mcr p15, 0, %0, c7, c14, 2\n\t"
                ::"r"(seg | ind):"memory");

            ind += 0x20;
        } while(ind < 0x400);
        seg += 0x40000000;
    } while(seg != 0);

    #else

    asmv("mcr p15, 0, %0, c7, c14, 0\n\t"
        ::"r"(0):"memory");

    #endif // ARM9

    DataSyncBarrier();
}

static inline void CACHE_WbInvDCRange(const void *base, u32 len) {
    u32 addr = (u32)base & ~0x1F;
    len >>= 5;

    do {
        asmv("mcr p15, 0, %0, c7, c14, 1"
            ::"r"(addr):"memory");

        addr += 0x20;
    } while(len--);

    DataSyncBarrier();
}

#endif
