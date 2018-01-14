#include <common.h>
#include <cpu.h>
#include <irq.h>

#include "hw/ndma.h"

void ndma_reset(void)
{
    for (int i = 0; i < 8; i++)
    {
        NDMA_CONTROL(i) = 0;
        irq_register(i, NULL);
    }

    *NDMA_GLOBAL_CONTROL = NDMA_GLOBAL_ROUNDROBIN(5) | NDMA_GLOBAL_ENABLE;
    return;
}

void ndma_stop(void)
{
    NDMA_CONTROL(0) = 0;
    return;
}

void ndma_copy_async(u32 *dst, const u32 *src, u32 len)
{
    NDMA_SRC(0) = (u32)src;
    NDMA_DST(0) = (u32)dst;
    NDMA_BLK_COUNT(0) = len / 4;
    NDMA_BLK_TIMING(0) = NDMA_BLK_TIMING_0 | 1;
    NDMA_CONTROL(0) = NDMA_DST_UPDATE_INC | NDMA_SRC_UPDATE_INC | NDMA_IMMEDIATE | NDMA_IRQ | NDMA_ENABLE;
    return;
}

void ndma_fill_async(u32 *dst, u32 fill, u32 len)
{
    NDMA_DST(0) = (u32)dst;
    NDMA_BLK_COUNT(0) = len / 4;
    NDMA_BLK_TIMING(0) = NDMA_BLK_TIMING_0 | 1;
    NDMA_FILL_DATA(0) = fill;
    NDMA_CONTROL(0) = NDMA_DST_UPDATE_INC | NDMA_SRC_UPDATE_NA | NDMA_IMMEDIATE | NDMA_IRQ | NDMA_ENABLE;
    return;
}

void ndma_copy(u32 *dst, const u32 *src, u32 len)
{
    ndma_copy_async(dst, src, len);
    while(NDMA_CONTROL(0) & NDMA_ENABLE)
        _wfi();
    return;
}

void ndma_fill(u32 *dst, u32 fill, u32 len)
{
    ndma_fill_async(dst, fill, len);
    while(NDMA_CONTROL(0) & NDMA_ENABLE)
        _wfi();
    return;
}

void ndma_copy_event_async(u32 *dst, const u32 *src, u32 blk_size, u32 len, u32 flags)
{
    NDMA_SRC(0) = (u32)src;
    NDMA_DST(0) = (u32)dst;
    NDMA_XFER_COUNT(0) = len;
    NDMA_BLK_COUNT(0) = blk_size;
    NDMA_BLK_TIMING(0) = NDMA_BLK_TIMING_0 | 1;
    NDMA_CONTROL(0) = flags | NDMA_IRQ | NDMA_ENABLE;
    return;
}

void ndma_copy_event(u32 *dst, const u32 *src, u32 blk_size, u32 len, u32 flags)
{
    ndma_copy_event_async(dst, src, blk_size, len, flags);
    while(NDMA_CONTROL(0) & NDMA_ENABLE)
        _wfi();
    return;
}
