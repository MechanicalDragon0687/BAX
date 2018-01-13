#pragma once

#include <common.h>

#define TIMER_FREQ   (67027964ULL)

#define TIMER_BASE   ((vu16*)(0x10003000))

#define TIMER_VAL(n) *(TIMER_BASE + ((n)*2))
#define TIMER_CNT(n) *(TIMER_BASE + 1 + ((n)*2))

#define TIMER_START   (BIT(7))
#define TIMER_IRQ     (BIT(6))
#define TIMER_COUNTUP (BIT(2))

void timer_reset(void);
void timer_start(u64 ticks, bool irq);
void timer_stop(void);
u64 timer_ticks(void);

static inline u64 timer_ms_to_ticks(u64 ms)
{
	return (TIMER_FREQ * ms) / 1000;
}

static inline u64 timer_ticks_to_ms(u64 ticks)
{
	return (ticks * 1000) / TIMER_FREQ;
}
