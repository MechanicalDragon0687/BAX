#include <common.h>

#include <arm/arm.h>
#include <arm/irq.h>
#include <hw/timer.h>

static DTCM timer_state timer_data[2];

static inline void timer_set_cnt(uint32_t t, uint16_t cnt_lo, uint16_t cnt_hi)
{
    *REG_TIMER_CNT(t*2+1) = cnt_hi;
    *REG_TIMER_CNT(t*2+0) = cnt_lo;
    return;
}

static inline void timer_set_val(uint32_t t, uint32_t ticks)
{
    *REG_TIMER_VAL(t*2+1) = ticks >> 16;
    *REG_TIMER_VAL(t*2+0) = ticks & 0xFFFF;
    return;
}

static inline uint32_t timer_get_val(uint32_t t)
{
    return (*REG_TIMER_VAL(t*2+1) << 16 | *REG_TIMER_VAL(t*2+0));
}

void ITCM timer_irq_dummy(void)
{
    return;
}

void ITCM timer_irq_handler(uint32_t xrq_id)
{
    uint32_t timer = (xrq_id - IRQ_TIMER(0)) / 2;
    int loops;

    (timer_data[timer].handler)();

    timer_set_cnt(timer, 0, 0);
    irq_ack(xrq_id);

    loops = timer_data[timer].loops;
    if (loops != 0) {
        if (loops > 0) {
            timer_data[timer].loops--;
        }

        timer_set_val(timer, timer_data[timer].tfreq);
        timer_set_cnt(timer,
                    TIMER_START | TIMER_PRESCALER,
                    TIMER_START | TIMER_IRQ | TIMER_COUNTUP | TIMER_PRESCALER);
    } else {
        timer_stop(timer);
    }
    return;
}

void timer_init(uint32_t timer, uint32_t freq, int loops, void (*hndl)(void))
{
    uint32_t ss;
    timer_stop(timer);
    ENTER_CRITICAL(ss);

    timer_data[timer].tfreq = ~(TIMER_FREQ / CLAMP(freq, 1, TIMER_FREQ));
    timer_data[timer].loops = loops;
    timer_data[timer].handler = (hndl!=NULL) ? hndl : timer_irq_dummy;

    timer_set_cnt(timer,
                TIMER_START | TIMER_PRESCALER,
                TIMER_START | TIMER_IRQ | TIMER_COUNTUP | TIMER_PRESCALER);
    irq_register(IRQ_TIMER(timer*2 + 1), timer_irq_handler);

    timer_set_val(timer, timer_data[timer].tfreq);
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

    timer_data[timer].tfreq = 0;
    timer_data[timer].loops = 0;
    timer_data[timer].handler = NULL;

    timer_set_cnt(timer, 0, 0);
    timer_set_val(timer, 0xFFFFFFFF);
    irq_deregister(IRQ_TIMER(timer*2 + 1));

    LEAVE_CRITICAL(ss);
    return;
}
