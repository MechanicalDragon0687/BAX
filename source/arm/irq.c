#include <common.h>

#include <arm/cpu.h>
#include <arm/irq.h>
#include <gfx/text.h>

static DTCM irq_handler (__irq_funcs[32]);

/* Default high level IRQ handler */
void ITCM irq_unhandled(uint32_t xrq_id)
{
    char str[2][TEXT_WIDTH];
    memset(str, 0, 2 * TEXT_WIDTH);
    msprintf(str[0], "Unexpected IRQ: %d", xrq_id);
    msprintf(str[1], "IRQ_IE = %X / IRQ_IF = %X", *REG_IRQ_IE, *REG_IRQ_IF);
    show_text(str, 2);

    irq_reset();

    while(1) {
        wfi();
    }
}

/* IRQ exception handler */
void ITCM __attribute__((interrupt("IRQ"))) xrq_irq(void)
{
    uint32_t xrq_id, ss;
    ENTER_CRITICAL(ss);
    xrq_id = __builtin_ctz(*REG_IRQ_IF);
    if (xrq_id <= 0x1F) {
        (__irq_funcs[xrq_id])(xrq_id);
    }
    LEAVE_CRITICAL(ss);
    return;
}

/* Register and enable an IRQ handler */
void irq_register(const unsigned int id, irq_handler func)
{
    if (id > 0x1F) {
        return;
    }

    irq_ack(id);
    *REG_IRQ_IE |= BIT(id);
    __irq_funcs[id] = func;
    return;
}

/* Disable an IRQ */
void irq_deregister(const unsigned int id)
{
    if (id > 0x1F) {
        return;
    }

    irq_ack(id);
    *REG_IRQ_IE &= ~(BIT(id));
    __irq_funcs[id] = irq_unhandled;
    return;
}

/* Reset all handlers IRQ */
void irq_reset(void)
{
    uint32_t ss;
    ENTER_CRITICAL(ss);
    *REG_IRQ_IE = 0;
    *REG_IRQ_IF = ~0;
    for (uint32_t i = 0; i < ARR_COUNT(__irq_funcs); i++) {
        __irq_funcs[i] = irq_unhandled;
    }
    LEAVE_CRITICAL(ss);
    return;
}
