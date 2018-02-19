#pragma once

#include <common.h>

#define CPU_CLK_FREQ  (268111856.0f)

#define REG_TIMER_LOAD    (*(vu32*)(0x17E00600))
#define REG_TIMER_COUNT   (*(vu32*)(0x17E00604))
#define REG_TIMER_CONTROL (*(vu32*)(0x17E00608))
#define REG_TIMER_STICKY  (*(vu32*)(0x17E0060C))


// Prescaler is always zero for simplicity
#define TIMER_ENABLE (BIT(0))
#define TIMER_RELOAD (BIT(1))
#define TIMER_IRQ    (BIT(2))


void TIMER_Reset(void);
void TIMER_Start(u32 ticks, bool arel, bool irq);
void TIMER_Stop(void);

static inline u32 TIMER_Ticks(void) {
    return REG_TIMER_COUNT;
}

#define TIMER_MS_FACTOR (((CPU_CLK_FREQ / 2000.0f) - 1.0f))
static inline u32 TIMER_MSToTicks(float ms) {
    return ms * TIMER_MS_FACTOR;
}

static inline float TIMER_TicksToMS(u32 ticks) {
    return (float)ticks / TIMER_MS_FACTOR;
}
