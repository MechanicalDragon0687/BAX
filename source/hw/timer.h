#pragma once

#include <common.h>
#include <arm/irq.h>

#define TIMER_FREQ (67027964 / 1024)
#define SECOND     (1000)

/* arbitrarily picked, prevents overflows */
#define MAX_PERIOD (10 * SECOND)

enum {
    DEFAULT_TIMER = 0,
    EXTRA_TIMER   = 1,
    NR_TIMERS
};

#define TIMER_IRQ_BASE  (8)

enum {
    TIMER_PRESCALER_NONE = (0),
    TIMER_PRESCALER_64   = (BIT(0)),
    TIMER_PRESCALER_256  = (BIT(1)),
    TIMER_PRESCALER_1024 = (BIT(1) | BIT(0))
};

#define TIMER_PRESCALER (TIMER_PRESCALER_1024)
#define TIMER_COUNTUP   (BIT(2))
#define TIMER_IRQ       (BIT(6))
#define TIMER_START     (BIT(7))

void timer_init(uint32_t timer, uint32_t period_ms, int loops, irq_handler hndl);
uint32_t timer_retrieve(uint32_t timer);
void timer_stop(uint32_t timer);
