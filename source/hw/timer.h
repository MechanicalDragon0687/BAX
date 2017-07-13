#pragma once

#include <common.h>
#include <arm/irq.h>

/* Physical Timer registers */
#define REG_TIMER_BASE   (0x10003000)
#define REG_TIMER_CNT(x) (((volatile uint16_t*)(REG_TIMER_BASE + 2 + (x)*4)))
#define REG_TIMER_VAL(x) (((volatile uint16_t*)(REG_TIMER_BASE + (x)*4)))

/* Timer controls */
#define TIMER_PRESCALER (BIT(1))
#define TIMER_COUNTUP   (BIT(2))
#define TIMER_IRQ       (BIT(6))
#define TIMER_START     (BIT(7))

/* Timer hardware constants */
/* Theoretically has a resolution of ~3.82*10^-6 seconds (1/TIMER_FREQ) */
#define TIMER_FREQ (67027964 / 256)

#define DEFAULT_TIMER (0)
#define EXTRA_TIMER   (1)

#define TIMER_PERIODIC (-1)
#define TIMER_ONESHOT  (0)

typedef struct {
    uint32_t tfreq;        /* Timer frequency converted to remaining ticks */
    int loops;             /* Remaining loops (only decreases if > 0) */
    void (*handler)(void); /* Function to call every time an IRQ is triggered */
} timer_state;

void timer_init(uint32_t timer, uint32_t freq, int loops, void (*hndl)(void));
uint32_t timer_retrieve(uint32_t timer);
void timer_stop(uint32_t timer);
