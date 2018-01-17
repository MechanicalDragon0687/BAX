#pragma once

#define MMU_AP_NA_NA (0x0000)
#define MMU_AP_RO_NA (0x8400)
#define MMU_AP_RW_NA (0x0400)
#define MMU_AP_RO_RO (0x8800)
#define MMU_AP_RW_RO (0x0800)
#define MMU_AP_RW_RW (0x0C00)

#define MMU_MEM_STRONGLY_ORDERED    (0x0000)
#define MMU_MEM_DEVICE_SHARED       (0x0004)
#define MMU_MEM_DEVICE_NONSHARED    (0x2000)
#define MMU_MEM_NONCACHEABLE        (0x1000)
#define MMU_MEM_WRITETHRU_NOWRALLOC (0x0008)
#define MMU_MEM_WRITEBACK_NOWRALLOC (0x000C)
#define MMU_MEM_WRITEBACK_WRALLOC   (0x100C)

#define MMU_PAGE_SHARED    (1 << 16)
#define MMU_PAGE_XN        (1 << 4)

#define MMU_PAGE_SECTION   (2)
#define MMU_PAGE_DOMAIN(x) ((x) << 5)
