#pragma once

/*
 None of these functions require a working stack
 Registers R0, R1, R2, R3 and R12 aren't guaranteed to be preserved
*/

#define BOOTROM_WFI                               (0xFFFF01A4)
#define BOOTROM_INVALIDATE_DCACHE                 (0xFFFF07F0)
#define BOOTROM_WRITEBACK_DCACHE                  (0xFFFF07FC)
#define BOOTROM_WRITEBACK_INVALIDATE_DCACHE       (0xFFFF0830)
#define BOOTROM_INVALIDATE_DCACHE_RANGE           (0xFFFF0868)
#define BOOTROM_WRITEBACK_DCACHE_RANGE            (0xFFFF0884)
#define BOOTROM_WRITEBACK_INVALIDATE_DCACHE_RANGE (0xFFFF08A8)
#define BOOTROM_INVALIDATE_ICACHE                 (0xFFFF0AB4)
#define BOOTROM_INVALIDATE_ICACHE_RANGE           (0xFFFF0AC0)
