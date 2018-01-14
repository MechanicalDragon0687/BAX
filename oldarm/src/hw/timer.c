#include <common.h>
#include "hw/timer.h"

void timer_reset(void)
{
    TIMER_CNT(0) = 0;
    TIMER_CNT(1) = 0;
    TIMER_CNT(2) = 0;
    TIMER_CNT(3) = 0;
    return;
}

void timer_start(u64 ticks, bool irq)
{
    ticks = ~ticks;
    TIMER_VAL(0) = ticks;
    TIMER_VAL(1) = ticks >> 16ULL;
    TIMER_VAL(2) = ticks >> 32ULL;
    TIMER_VAL(3) = ticks >> 48ULL;

    TIMER_CNT(3) = TIMER_START | (irq ? TIMER_IRQ : 0) | TIMER_COUNTUP;
    TIMER_CNT(2) = TIMER_START | TIMER_COUNTUP;
    TIMER_CNT(1) = TIMER_START | TIMER_COUNTUP;
    TIMER_CNT(0) = TIMER_START;
    return;
}

void timer_stop(void)
{
    timer_reset();
    return;
}

u64 timer_ticks(void)
{
    u64 ret = 0;
    ret |= TIMER_VAL(0);
    ret |= (u64)TIMER_VAL(1) << 16ULL;
    ret |= (u64)TIMER_VAL(2) << 32ULL;
    ret |= (u64)TIMER_VAL(3) << 48ULL;
    return ~ret;
}
