#include <common.h>

#include <arm/arm.h>
#include <arm/irq.h>
#include <hw/timer.h>

volatile DTCM timer_state timers[2];

void ITCM timer_irq_handler(uint32_t xrq_id)
{
    uint32_t timer = (xrq_id - IRQ_TIMER(0)) / 2;

    if (timers[timer].handler) {
        (timers[timer].handler)(xrq_id);
    }

    irq_ack(xrq_id);

    if (timers[timer].loops != 0) {
        if (timers[timer].loops > 0) {
            timers[timer].loops--;
        }

        timer_set_cnt(timer, 0, 0);
        timer_set_val(timer, timers[timer].ticks);
        timer_set_cnt(timer, TIMER_START | TIMER_PRESCALER, TIMER_START | TIMER_IRQ | TIMER_COUNTUP | TIMER_PRESCALER);

    } else {
        timer_stop(timer);
    }
    return;
}

void timer_init(uint32_t timer, uint32_t period_ms, int loops, irq_handler hndl)
{
    uint32_t ss;
    timer_stop(timer);
    ENTER_CRITICAL(ss);
    period_ms = CLAMP(period_ms, MIN_PERIOD, MAX_PERIOD);

    timers[timer].ticks = ~((period_ms * TIMER_FREQ) / SECOND);
    timers[timer].loops = loops;
    timers[timer].handler = hndl;

    timer_set_cnt(timer, TIMER_START | TIMER_PRESCALER, TIMER_START | TIMER_IRQ | TIMER_COUNTUP | TIMER_PRESCALER);
    irq_register(IRQ_TIMER(timer*2 + 1), timer_irq_handler);

    timer_set_val(timer, timers[timer].ticks);
    LEAVE_CRITICAL(ss);
    return;
}

uint32_t timer_retrieve(uint32_t timer)
{
    uint32_t ss, ret;
    ENTER_CRITICAL(ss);
    ret = timer_get_val(timer);
    LEAVE_CRITICAL(ss);
    return ret;
}

void timer_stop(uint32_t timer)
{
    uint32_t ss;
    ENTER_CRITICAL(ss);

    timers[timer].ticks = 0;
    timers[timer].loops = 0;
    timers[timer].handler = NULL;

    timer_set_cnt(timer, 0, 0);
    timer_set_val(timer, 0xFFFFFFFF);
    irq_deregister(IRQ_TIMER(timer*2 + 1));

    LEAVE_CRITICAL(ss);
    return;
}
