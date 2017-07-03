#include <common.h>

#include <arm/arm.h>
#include <arm/irq.h>
#include <hw/timer.h>

volatile uint16_t (*timer_data)[2] = (volatile uint16_t(*)[2])0x10003000;
volatile DTCM timer_state timers[2];

static inline void __timer_set_cfg(uint32_t i)
{
    i*=2;
    timer_data[i+1][REG_TIMER_CNT] = 0;
    timer_data[i+0][REG_TIMER_CNT] = 0;

    timer_data[i+1][REG_TIMER_VAL] = timers[i/2].ticks >> 16;
    timer_data[i+0][REG_TIMER_VAL] = timers[i/2].ticks;

    if (timers[i/2].loops != 0) {
        if (timers[i/2].handler) {
            timer_data[i+1][REG_TIMER_CNT] = TIMER_START | TIMER_IRQ | TIMER_COUNTUP | TIMER_PRESCALER;
        } else {
            timer_data[i+1][REG_TIMER_CNT] = TIMER_START | TIMER_COUNTUP | TIMER_PRESCALER;
        }
        timer_data[i+0][REG_TIMER_CNT] = TIMER_START | TIMER_PRESCALER;
    }
    return;
}

static inline bool __timer_dec_loop(uint32_t i)
{
    int loops = timers[i].loops;
    if (loops != 0) {
        if (loops > 0) {
            timers[i].loops = loops--;
        }
        return true;
    } else {
        return false;
    }
}

void ITCM timer_main_handler(uint32_t xrq_id)
{
    uint32_t timer_id = (xrq_id - TIMER_IRQ_BASE) / 2;

    (timers[timer_id].handler)(xrq_id);
    irq_ack(xrq_id);

    if (__timer_dec_loop(timer_id)) {
        __timer_set_cfg(timer_id);
    } else {
        timer_stop(timer_id);
    }
    return;
}

void timer_init(uint32_t t_id, uint32_t period_ms, int loops, irq_handler hndl)
{
    uint32_t ss;
    timer_stop(t_id);

    ss = irq_kill();

    if (period_ms > MAX_PERIOD) {
        period_ms = MAX_PERIOD;
    } else if (period_ms < MIN_PERIOD) {
        period_ms = MIN_PERIOD;
    }

    timers[t_id].ticks = ~((period_ms * TIMER_FREQ) / SECOND);
    timers[t_id].loops = loops;
    timers[t_id].handler = hndl;

    irq_register(t_id*2 + 1 + TIMER_IRQ_BASE, timer_main_handler);
    __timer_set_cfg(t_id);
    irq_restore(ss);
    return;
}

uint32_t timer_retrieve(uint32_t timer)
{
    uint32_t ss, ret;
    timer *= 2;
    ss = irq_kill();
    ret = (timer_data[timer+1][REG_TIMER_VAL] << 16) | (timer_data[timer+0][REG_TIMER_VAL]);
    irq_restore(ss);
    return ret;
}

void timer_stop(uint32_t timer)
{
    uint32_t ss = irq_kill();
    timers[timer].loops = 0;
    __timer_set_cfg(timer);
    irq_deregister(timer + 1 + TIMER_IRQ_BASE);
    irq_restore(ss);
    return;
}
