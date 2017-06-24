#include <common.h>

#include <arm/arm.h>
#include <arm/irq.h>
#include <gfx/text.h>

irq_handler DTCM (__irq_funcs[32]);

void irq_unhandled(uint32_t xrq_id)
{
    char str[2][TEXT_WIDTH];
    memset(str, 0, 2 * TEXT_WIDTH);
    msprintf(str[0], "Unexpected IRQ: %d", xrq_id);
    msprintf(str[1], "IRQ_IE = %X / IRQ_IF = %X", *IRQ_IE, *IRQ_IF);
    show_text(str, 2);

    irq_reset();

    while(1) {
        wfi();
    }
}

void ITCM __attribute__((interrupt("IRQ"))) xrq_irq(void)
{
    uint32_t xrq_id, cpsr;
    cpsr = read_cpsr();
    write_cpsr(cpsr | (IRQ_BIT | FIQ_BIT));
    xrq_id = __builtin_clz(*IRQ_IF);
    if (!(xrq_id & 0x20)) {
        xrq_id = 0x1F - xrq_id;
        (__irq_funcs[xrq_id])(xrq_id);
    }
    write_cpsr(cpsr);
    return;
}

bool irq_active(const unsigned int id)
{
    return (*IRQ_IE & BIT(id));
}

void irq_ack(const unsigned int id)
{
    if (id > 0x1F) {
        return;
    }

    *IRQ_IF = BIT(id);
    return;
}

void irq_register(const unsigned int id, irq_handler func)
{
    if (id > 0x1F) {
        return;
    }

    *IRQ_IE |= BIT(id);
    *IRQ_IF = ~0;
    __irq_funcs[id] = func;
    return;
}

void irq_deregister(const unsigned int id)
{
    if (id > 0x1F)
        return;

    *IRQ_IE &= ~(BIT(id));
    *IRQ_IF = ~0;
    __irq_funcs[id] = irq_unhandled;
    return;
}

void irq_reset(void)
{
    *IRQ_IE = 0;
    *IRQ_IF = ~0;
    for (uint32_t i = 0; i < sizeof(__irq_funcs)/sizeof(*__irq_funcs); i++) {
        __irq_funcs[i] = irq_unhandled;
    }
    return;
}
