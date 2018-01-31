#include <common.h>
#include <cpu.h>
#include "arm/irq.h"

isr_t _irq_handlers[IRQ_COUNT];

void irq_reset(void)
{
    // Clear the interrupt handler table
    memset(_irq_handlers, 0, sizeof(_irq_handlers));

    // Disable all interrupts, acknowledge all pending interrupts
    *IRQ_IE = 0;
    do {
        *IRQ_IF = ~0;
    } while(*IRQ_IF != 0);
    return;
}

void irq_register(u32 irqn, isr_t handler)
{
    if (irqn < IRQ_COUNT) {
        _irq_handlers[irqn] = handler;

        // Clear pending interrupt bit (just in case) and enable interrupt
        *IRQ_IF  = BIT(irqn);
        *IRQ_IE |= BIT(irqn);
    }
    return;
}

void irq_deregister(u32 irqn)
{
    if (irqn < IRQ_COUNT) {
        _irq_handlers[irqn] = NULL;
        *IRQ_IE &= ~BIT(irqn);
    }
    return;
}
