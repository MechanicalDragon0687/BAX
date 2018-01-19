#include <common.h>
#include <cpu.h>
#include "arm/irq.h"

isr_t _irq_handlers[IRQ_COUNT];
bool  _irq_enabled[IRQ_COUNT];

void irq_reset(void)
{
    // Clear the interrupt handler table
    memset(_irq_handlers, 0, sizeof(_irq_handlers));
    memset(_irq_enabled, 0, sizeof(_irq_enabled));

    // Disable all interrupts, clear all interrupt pending bits
    *IRQ_IE = 0;
    *IRQ_IF = ~0;
    return;
}

void irq_register(u32 irqn, isr_t handler)
{
    if (irqn < IRQ_COUNT)
    {
        _irq_handlers[irqn] = handler;
        _irq_enabled[irqn] = true;

        // Clear pending interrupt bit (just in case) and enable interrupt
        *IRQ_IF  = BIT(irqn);
        *IRQ_IE |= BIT(irqn);
    }
    return;
}

void irq_deregister(u32 irqn)
{
    if (irqn < IRQ_COUNT)
    {
        _irq_handlers[irqn] = NULL;
        _irq_enabled[irqn] = false;
        *IRQ_IE &= ~BIT(irqn);
    }
    return;
}
