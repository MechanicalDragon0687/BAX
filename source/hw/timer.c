#include <common.h>

#include <arm/arm.h>
#include <arm/irq.h>

#include <hw/timer.h>

volatile uint16_t (*timer_data)[2] = (volatile uint16_t(*)[2])0x10003000;

volatile DTCM irq_handler timer_hndl[NR_TIMERS];
volatile DTCM uint32_t timer_ticks[NR_TIMERS];
volatile DTCM int timer_loops[NR_TIMERS];

void ITCM timer_main_handler(uint32_t xrq_id)
{
    uint32_t timer_id = (xrq_id - TIMER_IRQ_BASE) / NR_TIMERS, tick;
    if (timer_id > 1) {
        abort();
    }

    (timer_hndl[timer_id])(xrq_id);
    irq_ack(xrq_id);

    if (timer_loops[timer_id] != 0) {
        if (timer_loops[timer_id] > 0) {
            timer_loops[timer_id]--;
        }

        tick = timer_ticks[timer_id];
        timer_id *= NR_TIMERS;
        timer_data[timer_id+1][1] = 0;
        timer_data[timer_id+0][1] = 0;

        timer_data[timer_id+1][0] = tick >> 16;
        timer_data[timer_id+0][0] = tick;

        timer_data[timer_id+1][1] = TIMER_START | TIMER_IRQ | TIMER_COUNTUP | TIMER_PRESCALER;
        timer_data[timer_id+0][1] = TIMER_START | TIMER_PRESCALER;
    } else {
        timer_stop(timer_id);
    }
    return;
}

void timer_init(uint32_t timer, uint32_t period_ms, int loops, irq_handler hndl)
{
    uint32_t tick_calc;

    if (timer > 1) {
        abort();
    }

    timer_stop(timer);

    if (period_ms > MAX_PERIOD) {
        period_ms = MAX_PERIOD;
    }

    tick_calc = ~((period_ms * TIMER_FREQ) / SECOND);

    timer_hndl[timer] = hndl;
    timer_ticks[timer] = tick_calc;
    timer_loops[timer] = loops;

    timer *= NR_TIMERS;
    timer_data[timer+1][1] = 0;
    timer_data[timer+0][1] = 0;

    timer_data[timer+1][0] = tick_calc >> 16;
    timer_data[timer+0][0] = tick_calc;

    if (hndl) {
        irq_register(timer + 1 + TIMER_IRQ_BASE, timer_main_handler);
        timer_data[timer+1][1] = TIMER_START | TIMER_IRQ | TIMER_COUNTUP | TIMER_PRESCALER;
    } else {
        irq_deregister(timer + 1 + TIMER_IRQ_BASE);
        timer_data[timer+1][1] = TIMER_START | TIMER_COUNTUP | TIMER_PRESCALER;
    }

    timer_data[timer+0][1] = TIMER_START | TIMER_PRESCALER;
    return;
}

uint32_t timer_retrieve(uint32_t timer)
{
    if (timer > 1) {
        abort();
    }
    timer *= NR_TIMERS;
    return ((timer_data[timer+1][0] << 16) | (timer_data[timer+0][0]));
}

void timer_stop(uint32_t timer)
{
    if (timer > 1) {
        abort();
    }
    timer *= NR_TIMERS;
    timer_data[timer+0][1] = 0;
    timer_data[timer+1][1] = 0;
    timer_loops[timer/NR_TIMERS] = 0;
    irq_deregister(timer + TIMER_IRQ_BASE);
    return;
}
