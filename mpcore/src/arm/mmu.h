#pragma once

#include <common.h>

#define MMU_PAGE_SIZE (0x100000)

// MMU Section Descriptor
typedef struct {
    u32 type : 2;
    u32 bufferable : 1;
    u32 cachable : 1;
    u32 xn : 1;
    u32 domain : 4;
    u32 ecc_enabled : 1;
    u32 ap : 2;
    u32 tex : 3;
    u32 apx : 1;
    u32 shared : 1;
    u32 notg : 1;
    u32 : 2;
    u32 address : 12;
} mmu_sect_t;

// MMU Access Permissions
typedef enum {
    MMU_AP_NA_NA = 0,
    MMU_AP_RO_NA,
    MMU_AP_RW_NA,
    MMU_AP_RO_RO,
    MMU_AP_RW_RO,
    MMU_AP_RW_RW,
    MMU_AP_INVALID
} mmu_ap_t;

// MMU Memory Attributes
typedef enum {
    MMU_STRONGLY_ORDERED = 0,
    MMU_DEVICE_SHARED,
    MMU_DEVICE_NONSHARED,
    MMU_NONCACHABLE,
    MMU_WRITETHRU_NOWRALLOC,
    MMU_WRITEBACK_NOWRALLOC,
    MMU_WRITEBACK_WRALLOC,
    MMU_MEMATTR_INVALID
} mmu_mem_attr_t;

// Reinitialize the MMU
void mmu_reset(void);

// Invalidate the MMU Translation Lookaside Buffer
void mmu_invalidate_tlb(void);


// Set the MMU Translation Table Base Register
void mmu_set_ttbr(u32 *tt, u32 cfg);

// Get the MMU Translation Table Base Register
u32 *mmu_get_ttbr(void);


// Set the MMU Domain Access Control Register
void mmu_set_dacr(u32 dacr);

// Get the MMU Domain Access Control Register
u32  mmu_get_dacr(void);


// Create a MMU Section Descriptor
// pa = Physical Address
// s  = Shared
// d  = Domain
// a  = Memory Attributes
// p  = Access Permissions
// xn = eXecute Never
mmu_sect_t mmu_make_desc(u32 pa, bool s, u8 d, mmu_mem_attr_t a, mmu_ap_t p, bool xn);
