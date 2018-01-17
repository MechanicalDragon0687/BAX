#include <common.h>
#include <cache.h>

#ifdef ARM9
void _dbt(void)
{
    return;
}

void _dpb(void)
{
    return;
}

void _dmb(void)
{
    return;
}
#else
void _dbt(void)
{
    __asm__ __volatile__(
        "mcr p15, 0, %0, c7, c5, 6\n\t"
        : : "r"(0) : "memory"
    );
    return;
}

void _dpb(void)
{
    __asm__ __volatile__(
        "mcr p15, 0, %0, c7, c5, 4"
        : : "r"(0) : "memory"
    );
    return;
}

void _dmb(void)
{
    __asm__ __volatile__(
        "mcr p15, 0, %0, c7, c10, 5\n\t"
        : : "r"(0) : "memory"
    );
    return;
}
#endif

void _dsb(void)
{
    __asm__ __volatile__(
        "mcr p15, 0, %0, c7, c10, 4\n\t"
        : : "r"(0) : "memory"
    );
    return;
}

void _invalidate_IC(void)
{
    __asm__ __volatile__(
        "mcr p15, 0, %0, c7, c5, 0\n\t"
        : : "r"(0) : "memory"
    );
    _dsb();
    _dpb();
    return;
}

void _invalidate_IC_range(const void *base, u32 len)
{
    u32 addr = (u32)base & ~0x1F;
    len >>= 5;
    do
    {
        __asm__ __volatile__(
            "mcr p15, 0, %0, c7, c5, 1\n\t"
            : : "r"(addr) : "memory"
        );
        addr += 0x20;
    } while(len--);
    _dbt();
    _dsb();
    _dpb();
    return;
}

void _invalidate_DC(void)
{
    __asm__ __volatile__(
        "mcr p15, 0, %0, c7, c6, 0\n\t"
        : : "r"(0) : "memory"
    );
    _dsb();
    return;
}

void _invalidate_DC_range(const void *base, u32 len)
{
    u32 addr = (u32)base & ~0x1F;
    len >>= 5;
    do
    {
        __asm__ __volatile__(
            "mcr p15, 0, %0, c7, c6, 1"
            : : "r"(addr) : "memory"
        );
        addr += 0x20;
    } while(len--);
    _dsb();
    return;
}

void _writeback_DC(void)
{
    #ifdef ARM9
    u32 seg=0, ind;
    do
    {
        ind=0;
        do
        {
            __asm__ __volatile__(
                "mcr p15, 0, %0, c7, c10, 2\n\t"
                : : "r"(seg | ind) : "memory"
            );
            ind += 0x20;
        } while(ind < 0x400);
        seg += 0x40000000;
    } while(seg);
    #else
    __asm__ __volatile__(
        "mcr p15, 0, %0, c7, c10, 0\n\t"
        : : "r"(0) : "memory"
    );
    #endif
    _dsb();
    return;
}

void _writeback_DC_range(const void *base, u32 len)
{
    u32 addr = (u32)base & ~0x1F;
    len >>= 5;
    do
    {
        __asm__ __volatile__(
            "mcr p15, 0, %0, c7, c10, 1"
            : : "r"(addr) : "memory"
        );
        addr += 0x20;
    } while(len--);
    _dsb();
    return;
}

void _writeback_invalidate_DC(void)
{
    #ifdef ARM9
    u32 seg = 0, ind;
    do
    {
        ind = 0;
        do
        {
            __asm__ __volatile__(
                "mcr p15, 0, %0, c7, c14, 2\n\t"
                : : "r"(seg|ind) : "memory"
            );
            ind += 0x20;
        } while(ind < 0x400);
        seg += 0x40000000;
    } while(seg);
    #else
    __asm__ __volatile__(
        "mcr p15, 0, %0, c7, c14, 0\n\t"
        : : "r"(0) : "memory"
    );
    #endif
    _dsb();
    return;
}

void _writeback_invalidate_DC_range(const void *base, u32 len)
{
    u32 addr = (u32)base & ~0x1F;
    len >>= 5;
    do
    {
        __asm__ __volatile__(
            "mcr p15, 0, %0, c7, c14, 1"
            : : "r"(addr) : "memory"
        );
        addr += 0x20;
    } while(len--);
    _dsb();
    return;
}

void _fuck_caches(void)
{
    #ifdef ARM9
    _invalidate_DC();
    _invalidate_IC();
    _dsb();
    #else
    __asm__ __volatile__(
        "mcr p15, 0, %0, c7, c7, 0\n\t"
        : : "r"(0) : "memory"
    );
    _dsb();
    _dmb();
    #endif
    return;
}
