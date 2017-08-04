#include <common.h>

#include <cpu.h>
#include <arm/irq.h>

static irq_handler __irq_funcs[IRQ_COUNT] = {NULL};

#define IRQ_BASE ((u32*)0x8000000)

/* IRQ exception handler */
void __attribute__((interrupt("IRQ"))) xrq_irq(void)
{
    u32 xrq_id, ss;
    ENTER_CRITICAL(ss);
    xrq_id = __builtin_ctz(*REG_IRQ_IF);
    if (xrq_id <= IRQ_COUNT) {
        (__irq_funcs[xrq_id])(xrq_id);
    }
    irq_ack(xrq_id);
    LEAVE_CRITICAL(ss);
    return;
}

/* Register and enable an IRQ handler */
void irq_register(unsigned int id, irq_handler func)
{
    if (id > IRQ_COUNT) {
        return;
    }

    irq_ack(id);
    __irq_funcs[id] = func;
    *REG_IRQ_IE |= BIT(id);
    return;
}

/* Disable an IRQ */
void irq_deregister(unsigned int id)
{
    if (id > IRQ_COUNT) {
        return;
    }

    irq_ack(id);
    __irq_funcs[id] = NULL;
    *REG_IRQ_IE &= ~(BIT(id));
    return;
}

/* Reset all IRQ handlers and install the vector */
void irq_reset(void)
{
    u32 ss;
    ENTER_CRITICAL(ss);
    *REG_IRQ_IE = 0;
    *REG_IRQ_IF = ~0;
    memset(__irq_funcs, 0, sizeof(__irq_funcs));
    IRQ_BASE[1] = (u32)xrq_irq;
    IRQ_BASE[0] = 0xE51FF004;
    LEAVE_CRITICAL(ss);
    return;
}
