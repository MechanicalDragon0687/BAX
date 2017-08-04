/*
 to be honest I haven't even actually tested this stuff
 should work on paper but idk, I'll leave it for later
*/

#include <common.h>
#include <gic.h>

#define IRQ_BASE ((vu32*)0x1FFFFFA0)

irq_handler handler_table[MAX_IRQ];

void __attribute__((interrupt("IRQ"))) gic_irq_handler(void)
{
    u32 ss, xrq;
    ENTER_CRITICAL(ss);
    xrq = *GIC_IRQACK;
    if (xrq < MAX_IRQ && handler_table[xrq]) {
        (handler_table[xrq])(xrq);
    }
    *GIC_IRQEND = xrq;
    LEAVE_CRITICAL(ss);
    return;
}

void gic_configure(u32 irq_id, irq_handler hndl)
{
    u32 ss;
    ENTER_CRITICAL(ss);
    handler_table[irq_id] = hndl;
    DIC_CLRENABLE[irq_id/32] |= BIT(irq_id & 0x1F);
    DIC_SETENABLE[irq_id/32] |= BIT(irq_id & 0x1F);
    DIC_PROCTGT[irq_id] = 1;
    LEAVE_CRITICAL(ss);
    return;
}

void gic_reset(void)
{
    u32 ss, reg_off;
    ENTER_CRITICAL(ss);

    *GIC_CONTROL = 0;
    *GIC_PRIOMASK = ~0;

    for (int i = 0; i < (BIT(9)-1); i++) {
        *GIC_IRQEND |= i;
    }

    *DIC_CONTROL = 0;
    for (int i = 0; i < (0x20/4); i++) {
        DIC_CLRENABLE[i] = ~0;
        DIC_PRIORITY[i] = 0;
    }

    *DIC_CONTROL = 1;
    *GIC_CONTROL = 1;

    IRQ_BASE[1] = (u32)gic_irq_handler;
    IRQ_BASE[0] = 0xE51FF004;

    LEAVE_CRITICAL(ss);
    return;
}
