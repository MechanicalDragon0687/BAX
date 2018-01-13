#include <common.h>
#include <cpu.h>
#include <irq.h>

static isr_t _irq_handlers[IRQ_COUNT];
static bool _irq_enabled[IRQ_COUNT];


NOINLINE void irq_reset(void)
{
    // Clear the interrupt handler table
    memset(_irq_handlers, 0, sizeof(_irq_handlers));
    memset(_irq_enabled, 0, sizeof(_irq_enabled));

    #ifdef ARM9
    // Disable all interrupts, clear all interrupt pending bits
    IRQ_BASE[0] = 0;
    IRQ_BASE[1] = ~0;
    #else
    // Disable GIC & DIC interfaces
    *GIC_CONTROL = 0;
    *DIC_CONTROL = 0;

    // Disable the priority mask
    *GIC_PRIOMASK = 0xF0;

    // All priority bits are compared for pre-emption
    *GIC_POI = 0x03;

    // Signal end of interrupt for all interrupts
    for (int i = 0; i < IRQ_COUNT; i++)
        *GIC_IRQEND = i;

    // Clear all interrupt enable bits and set max priority (zero)
    for (int i = 0; i < 4; i++)
    {
        DIC_CLRENABLE[i]  = ~0;
        DIC_CLRPENDING[i] = ~0;
        DIC_PRIORITY[i]   = 0;
    }

    // Set all interrupts to be triggered on rising edge
    for (int i = 0; i < 8; i++)
        DIC_CFGREG[i] = ~0;

    // Set CPU target to be no CPU for all interrupts (except SW)
    for (int i = 32; i < IRQ_COUNT; i++)
        DIC_TARGETPROC[i] = 0;

    // Enable DIC & GIC interfaces
    *DIC_CONTROL = 1;
    *GIC_CONTROL = 1;

    // Clear all interrupt pending bits until we get the spurious interrupt
    u32 irq_s;
    while((irq_s = *GIC_PENDING) != 0x3FF) *GIC_IRQEND = irq_s;
    #endif
    return;
}

NOINLINE void irq_ack(u32 irqn)
{
    if (irqn < IRQ_COUNT)
    {
        #ifdef ARM9
        IRQ_BASE[1] = BIT(irqn);
        #else
        *GIC_IRQEND = irqn;
        #endif
    }
    return;
}

NOINLINE u32 irq_pending(void)
{
    u32 ret;
    #ifdef ARM9
    ret = __builtin_ctz(IRQ_BASE[1]);
    #else
    ret = *GIC_IRQACK;
    #endif
    return ret;
}

NOINLINE void irq_register(u32 irqn, isr_t handler)
{
    if (irqn < IRQ_COUNT)
    {
        _irq_handlers[irqn] = handler;
        _irq_enabled[irqn] = true;

        #ifdef ARM9
        // Clear pending interrupt bit (just in case) and enable interrupt
        *IRQ_BASE |= BIT(irqn);
        #else
        DIC_CLRENABLE[irqn >> 5] |= BIT(irqn & 0x1F);
        DIC_SETENABLE[irqn >> 5] |= BIT(irqn & 0x1F);
        DIC_TARGETPROC[irqn] = 1;
        #endif
    }
    return;
}

NOINLINE void irq_deregister(u32 irqn)
{
    if (irqn < IRQ_COUNT)
    {
        _irq_handlers[irqn] = NULL;
        _irq_enabled[irqn] = false;
        #ifdef ARM9
        IRQ_BASE[0] &= ~BIT(irqn);
        #else
        DIC_CLRENABLE[irqn >> 5] |= BIT(irqn);
        DIC_TARGETPROC[irqn] = 0;
        #endif
    }
    return;
}

NOINLINE isr_t irq_handler(u32 irqn)
{
    if (irqn < IRQ_COUNT)
        return _irq_handlers[irqn];
    return NULL;
}

NOINLINE bool irq_enabled(u32 irqn)
{
    return ((irqn < IRQ_COUNT) && _irq_enabled[irqn]);
}
