#include <common.h>

#include "hw/timer.h"

void TIMER_Reset(void)
{
    REG_TIMER_CONTROL = 0;
    REG_TIMER_LOAD = 0;
    REG_TIMER_STICKY = 1;
    return;
}

void TIMER_Start(u32 ticks, bool arel, bool irq)
{
    u32 tcnt;
    REG_TIMER_LOAD = ticks;

    tcnt = TIMER_ENABLE;
    if (irq) tcnt |= TIMER_IRQ;
    if (arel) tcnt |= TIMER_RELOAD;

    REG_TIMER_CONTROL = tcnt;
    return;
}

void TIMER_Stop(void)
{
    REG_TIMER_CONTROL = 0;
    REG_TIMER_STICKY = 1;
    return;
}
