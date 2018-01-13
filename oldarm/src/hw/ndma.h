#pragma once

#include <common.h>

#define NDMA_GLOBAL_CONTROL ((vu32*)(0x10002000))

#define NDMA_GLOBAL_ENABLE        (BIT(0))
#define NDMA_GLOBAL_ROUNDROBIN(n) (BIT(31) | ((n) & 0xF) << 16)

#define NDMA_SRC(n)         *(NDMA_GLOBAL_CONTROL + ((n)*7) + 1)
#define NDMA_DST(n)         *(NDMA_GLOBAL_CONTROL + ((n)*7) + 2)
#define NDMA_XFER_COUNT(n)  *(NDMA_GLOBAL_CONTROL + ((n)*7) + 3)
#define NDMA_BLK_COUNT(n)   *(NDMA_GLOBAL_CONTROL + ((n)*7) + 4)
#define NDMA_BLK_TIMING(n)  *(NDMA_GLOBAL_CONTROL + ((n)*7) + 5)
#define NDMA_FILL_DATA(n)   *(NDMA_GLOBAL_CONTROL + ((n)*7) + 6)
#define NDMA_CONTROL(n)     *(NDMA_GLOBAL_CONTROL + ((n)*7) + 7)

#define NDMA_BLK_TIMING_0  (0)
#define NDMA_BLK_TIMING_4  (1 << 16)
#define NDMA_BLK_TIMING_16 (2 << 16)
#define NDMA_BLK_TIMING_64 (3 << 16)


#define NDMA_DST_UPDATE_INC (0 << 10)
#define NDMA_DST_UPDATE_DEC (1 << 10)
#define NDMA_DST_UPDATE_FIX (2 << 10)
#define NDMA_DST_RELOAD     (BIT(12))

#define NDMA_SRC_UPDATE_INC (0 << 13)
#define NDMA_SRC_UPDATE_DEC (1 << 13)
#define NDMA_SRC_UPDATE_FIX (2 << 13)
#define NDMA_SRC_UPDATE_NA  (3 << 13)
#define NDMA_SRC_RELOAD     (BIT(15))

#define NDMA_BURST_COUNT(n) ((n) << 16)

#define NDMA_STARTUP_TIMER0   (0 << 24)
#define NDMA_STARTUP_TIMER1   (1 << 24)
#define NDMA_STARTUP_TIMER2   (2 << 24)
#define NDMA_STARTUP_TIMER3   (3 << 24)
#define NDMA_STARTUP_CTRCARD0 (4 << 24)
#define NDMA_STARTUP_CTRCARD1 (5 << 24)
#define NDMA_STARTUP_EMMC     (7 << 24)
#define NDMA_STARTUP_AES_IN   (8 << 24)
#define NDMA_STARTUP_AES_OUT  (9 << 24)
#define NDMA_STARTUP_SHA      (10 << 24)
#define NDMA_STARTUP_AES_SHA  (15 << 24)

#define NDMA_IMMEDIATE      (BIT(28))
#define NDMA_REPEATING      (BIT(29))

#define NDMA_IRQ            (BIT(30))
#define NDMA_ENABLE         (BIT(31))


void ndma_reset(void);
void ndma_stop(void);

void ndma_copy_async(u32 *dst, const u32 *src, u32 len);
void ndma_copy(u32 *dst, const u32 *src, u32 len);

void ndma_fill_async(u32 *dst, u32 fill, u32 len);
void ndma_fill(u32 *dst, u32 fill, u32 len);

void ndma_copy_event_async(u32 *dst, const u32 *src, u32 blk_size, u32 len, u32 flags);
void ndma_copy_event(u32 *dst, const u32 *src, u32 blk_size, u32 len, u32 flags);
