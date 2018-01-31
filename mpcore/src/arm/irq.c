#include <common.h>
#include <cpu.h>

#include "arm/bugcheck.h"
#include "arm/irq.h"

isr_t _lirq_handlers[2][32];
isr_t _irq_handlers[96];

void irq_reset(void)
{
    u32 core = _coreID(), irq_s;

    // Disable local interrupt distributor
    *GIC_CONTROL = 0;
    memset(_lirq_handlers[core], 0, sizeof(_lirq_handlers[core]));

    // Setup hardware interrupts
    if (core == 0) {
        *DIC_CONTROL = 0;
        memset(_irq_handlers, 0, sizeof(_irq_handlers));

        for (int i = 0; i < 4; i++) {
            DIC_CLRENABLE[i]  = ~0;
            DIC_CLRPENDING[i] = ~0;
        }

        for (int i = 0; i < 32; i++)
            DIC_PRIORITY[i] = 0;

        for (int i = 32; i < 128; i++)
            DIC_TARGETPROC[i] = 0;

        for (int i = 0; i < 8; i++)
            DIC_CFGREG[i] = ~0;

        *DIC_CONTROL = 1;
    }

    // Setup local interrupts
    DIC_CLRENABLE[0] = ~0;

    for (int i = 0; i < 32; i++)
        DIC_PRIORITY[i] = 0;

    for (int i = 0; i < 2; i++)
        DIC_CFGREG[i] = ~0;

    // All priority bits are compared for pre-emption
    *GIC_POI = 3;

    // All 4 bits are used for the priority mask
    *GIC_PRIOMASK = 0xF0;

    // Enable the local distributor
    *GIC_CONTROL = 1;

    // Send end of interrupt until the spurious interrupt shows up
    do {
        irq_s = *GIC_PENDING;
        *GIC_IRQEND = irq_s;
    } while(irq_s != 0x3FF);
    return;
}

void irq_register(u32 irqn, isr_t handler, u32 core)
{
    if (irqn >= 128 || core >= 2)
        bugcheck("IRQ_REGISTER", (u32[]){irqn, core}, 2);

    if (irqn < 32)
        _lirq_handlers[core][irqn] = handler;
    else
        _irq_handlers[irqn - 32] = handler;

    DIC_CLRPENDING[irqn >> 5] |= BIT(irqn & 0x1F);
    DIC_SETENABLE[irqn >> 5]  |= BIT(irqn & 0x1F);
    DIC_TARGETPROC[irqn] = core + 1;
    return;
}

void irq_deregister(u32 irqn, u32 core)
{
    if (irqn >= 128 || core >= 2)
        bugcheck("IRQ_DEREGISTER", (u32[]){irqn, core}, 2);

    if (irqn < 32)
        _lirq_handlers[core][irqn] = NULL;
    else
        _irq_handlers[irqn - 32] = NULL;

    DIC_CLRPENDING[irqn >> 5] |= BIT(irqn & 0x1F);
    DIC_CLRENABLE[irqn >> 5]  |= BIT(irqn & 0x1F);
    DIC_TARGETPROC[irqn] = 0;
    return;
}
