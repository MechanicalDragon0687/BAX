#pragma once

#include <common.h>
#include <arm/irq.h>

/* 4 physical 16 bit timers -> 2 logical 32 bit timers */

/* Physical registers */
#define REG_TIMER_BASE   (0x10003000)
#define REG_TIMER_CNT(x) (((volatile uint16_t*)(REG_TIMER_BASE + 2 + (x)*4)))
#define REG_TIMER_VAL(x) (((volatile uint16_t*)(REG_TIMER_BASE + (x)*4)))

/* Logical functions */
static inline void timer_set_cnt(uint32_t t, uint16_t cnt_lo, uint16_t cnt_hi)
{
    t*=2;
    *REG_TIMER_CNT(t+1) = cnt_hi;
    *REG_TIMER_CNT(t+0) = cnt_lo;
    return;
}

static inline void timer_set_val(uint32_t t, uint32_t ticks)
{
    t*=2;
    *REG_TIMER_VAL(t+1) = ticks >> 16;
    *REG_TIMER_VAL(t+0) = ticks & 0xFFFF;
    return;
}

static inline uint32_t timer_get_val(uint32_t t)
{
    t*=2;
    return (*REG_TIMER_VAL(t+1) << 16 | *REG_TIMER_VAL(t+0));
}

/* Timer-related constants */
#define TIMER_FREQ (67027964 / 1024)
#define SECOND     (1000)
#define MAX_PERIOD (20 * SECOND)
#define MIN_PERIOD (SECOND / 100)

#define DEFAULT_TIMER (0)
#define EXTRA_TIMER   (1)

#define TIMER_PERIODIC (-1)
#define TIMER_ONESHOT  (1)

typedef struct {
    uint32_t ticks;
    /*
     Ticks that will be set on every startup.
     An IRQ will be triggered once the logical
     timer reaches 0xFFFFFFFF
    */
    int loops;           /* Remaining loops (only decreases if > 0) */
    irq_handler handler; /* Function to call every time an IRQ is triggered (NOTE: must be valid). */
} timer_state;

#define TIMER_PRESCALER (BIT(1) | BIT(0))
#define TIMER_COUNTUP   (BIT(2))
#define TIMER_IRQ       (BIT(6))
#define TIMER_START     (BIT(7))

void timer_init(uint32_t timer, uint32_t period_ms, int loops, irq_handler hndl);
uint32_t timer_retrieve(uint32_t timer);
void timer_stop(uint32_t timer);
