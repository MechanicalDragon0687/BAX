#include <common.h>
#include "arm/mmu.h"

/*
 THESE **MUST** BE CALLED WITH THE MMU DISABLED!
*/

void mmu_reset(void)
{
    __asm__ __volatile__ (
        "mcr p15, 0, %0, c2, c0, 0\n\t" // clear TTBR0
        "mcr p15, 0, %0, c2, c0, 1\n\t" // clear TTBR1
        "mcr p15, 0, %0, c2, c0, 2\n\t" // always use TTBR 0 (hail ARM926 \i/)
        "mcr p15, 0, %1, c3, c0, 0\n\t" // set all domains to Client (checked)
        : : "r"(0), "r"(0x55555555) : "memory"
    );
    return;
}

void mmu_invalidate_tlb(void)
{
    __asm__ __volatile__ (
        "mcr p15, 0, %0, c8, c7, 0"
        : : "r"(0) : "memory"
    );
    return;
}

void mmu_set_ttbr(u32 *tt, u32 cfg)
{
    cfg &= 0x1A;
    cfg |= (u32)tt & 0xFFFFC000;
    __asm__ __volatile__ (
        "mcr p15, 0, %0, c2, c0, 0\n\t"
        : : "r"(cfg) : "memory"
    );
    return;
}

u32 *mmu_get_ttbr(void)
{
    u32 *tt;
    __asm__ __volatile__ (
        "mrc p15, 0, %0, c2, c0, 0\n\t"
        : "=r"(tt)
    );
    return tt;
}

void mmu_set_dacr(u32 dacr)
{
    __asm__ __volatile__ (
        "mcr p15, 0, %0, c3, c0, 0\n\t"
        : : "r"(dacr)
    );
    return;
}

u32 mmu_get_dacr(void)
{
    u32 dacr;
    __asm__ __volatile__ (
        "mrc p15, 0, %0, c3, c0, 0\n\t"
        : "=r"(dacr)
    );
    return dacr;
}

/* Contains AP and APX fields */
const u8 mmu_aps[MMU_AP_INVALID][2] =
{
	[MMU_AP_NA_NA] = {0, 0},
	[MMU_AP_RO_NA] = {1, 1},
	[MMU_AP_RW_NA] = {1, 0},
	[MMU_AP_RO_RO] = {2, 1},
	[MMU_AP_RW_RO] = {2, 0},
	[MMU_AP_RW_RW] = {3, 0}
};

/* Contains B, C and TEX fields */
const u8 mmu_mem_attrs[MMU_MEMATTR_INVALID][3] =
{
	[MMU_STRONGLY_ORDERED]    = {0, 0, 0},
	[MMU_DEVICE_SHARED]       = {1, 0, 0},
	[MMU_DEVICE_NONSHARED]    = {0, 0, 2},
	[MMU_NONCACHABLE]         = {0, 0, 1},
	[MMU_WRITETHRU_NOWRALLOC] = {0, 1, 0},
	[MMU_WRITEBACK_NOWRALLOC] = {1, 1, 0},
	[MMU_WRITEBACK_WRALLOC]   = {1, 1, 1}
};

mmu_sect_t mmu_make_desc(u32 pa, bool s, u8 d, mmu_mem_attr_t a, mmu_ap_t p, bool xn)
{
    COMPILE_ASSERT(sizeof(mmu_sect_t) == 4);

	mmu_sect_t ret = {0};
	if (a >= MMU_MEMATTR_INVALID || p >= MMU_AP_INVALID)
		return ret;

	ret.type = 2;
	ret.bufferable = mmu_mem_attrs[a][0];
	ret.cachable = mmu_mem_attrs[a][1];
	ret.xn = (xn == true) ? 1 : 0;
	ret.domain = d;
	ret.ecc_enabled = 0;
	ret.ap = mmu_aps[p][0];
	ret.tex = mmu_mem_attrs[a][2];
	ret.apx = mmu_aps[p][1];
	ret.shared = (s == true) ? 1 : 0;
	ret.notg = 0;
	ret.address = pa >> 20;

	return ret;
}
