#pragma once

#include <common.h>

#define CPU_CLK_FREQ  (268111856.0f)

#define TIMER_LOAD    ((vu32*)(0x17E00600))
#define TIMER_COUNT   ((vu32*)(0x17E00604))
#define TIMER_CONTROL ((vu32*)(0x17E00608))
#define TIMER_STICKY  ((vu32*)(0x17E0060C))

// Prescaler is always zero for simplicity
#define TIMER_ENABLE       (BIT(0))
#define TIMER_RELOAD       (BIT(1))
#define TIMER_IRQ          (BIT(2))


// Reinitialize the MPcore timer hardware
void timer_reset(void);

// Start the timer, with the possibility of auto reload and IRQ trigger on underflow
void timer_start(u32 ticks, bool arel, bool irq);

// Stop the timer
void timer_stop(void);

// Get the timer ticks
u32 timer_ticks(void);

#define TIMER_CONV_FACTOR (((CPU_CLK_FREQ / 2000.0f) - 1.0f))

// Convert miliseconds to timer ticks
static inline u32 timer_ms_to_ticks(float ms)
{
	return ms * TIMER_CONV_FACTOR;
}

// Convert timer ticks to miliseconds
static inline float timer_ticks_to_ms(u32 ticks)
{
	return (float)ticks / TIMER_CONV_FACTOR;
}
