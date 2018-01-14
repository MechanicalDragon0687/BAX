#include <common.h>
#include "hw/timer.h"

void timer_reset(void)
{
    *TIMER_CONTROL = 0;
    *TIMER_LOAD = 0;
    *TIMER_STICKY = 1;
    return;
}

void timer_start(u32 ticks, bool arel, bool irq)
{
    u32 tcnt;
    timer_reset();
    *TIMER_LOAD = ticks;

    tcnt = TIMER_ENABLE;
    if (irq) tcnt |= TIMER_IRQ;
    if (arel) tcnt |= TIMER_RELOAD;

    *TIMER_CONTROL = tcnt;
    return;
}

void timer_stop(void)
{
    *TIMER_CONTROL = 0;
    *TIMER_STICKY = 1;
    return;
}

u32 timer_ticks(void)
{
    return *TIMER_COUNT;
}
