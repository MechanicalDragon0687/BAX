#pragma once

#include <common.h>

// Drain Branch Target Cache
void _dbt(void);

// Drain Prefetch Buffer
void _dpb(void);

// Data Memory Barrier
void _dmb(void);

// Data Sync Barrier / Drain Write Buffer
void _dsb(void);


// Invalidate the entire Instruction Cache
void _invalidate_IC(void);

// Invalidate Instruction Cache in the range <base, base + len>
void _invalidate_IC_range(const void *base, u32 len);


// Invalidate the entire Data Cache
void _invalidate_DC(void);

// Invalidate Data Cache in the range <base, base + len>
void _invalidate_DC_range(const void *base, u32 len);


// Writeback the entire Data Cache
void _writeback_DC(void);

// Writeback the Data Cache in the range <base, base + len>
void _writeback_DC_range(const void *base, u32 len);


// Writeback and invalidate the entire Data Cache
void _writeback_invalidate_DC(void);

// Writeback and invalidate the Data Cache in the range <base, base + len>
void _writeback_invalidate_DC_range(const void *base, u32 len);


// TODO: Find a better function name
// Magic voodoo, use only when absolutely necessary
void _fuck_caches(void);

#ifdef CACHE_CODE
#include <cache.c>
#endif
