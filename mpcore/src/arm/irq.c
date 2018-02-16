#include <common.h>
#include <cpu.h>

#include "arm/bug.h"
#include "arm/irq.h"

ISR IRQ_LocalHandlers[2][32];
ISR IRQ_GlobalHandlers[96];

void IRQ_Reset(void)
{
    u32 core = CPU_CoreID(), irq_s;

    REG_GIC_CONTROL = 0;
    memset(IRQ_LocalHandlers[core], 0, sizeof(IRQ_LocalHandlers[core]));

    if (core == 0) {
        REG_DIC_CONTROL = 0;
        memset(IRQ_GlobalHandlers, 0, sizeof(IRQ_GlobalHandlers));
        for (int i = 0; i < 4; i++) {
            REG_DIC_CLRENABLE[i]  = ~0;
            REG_DIC_CLRPENDING[i] = ~0;
        }
        for (int i = 0; i < 32; i++)   REG_DIC_PRIORITY[i] = 0;
        for (int i = 32; i < 128; i++) REG_DIC_TARGETPROC[i] = 0;
        for (int i = 0; i < 8; i++)    REG_DIC_CFGREG[i] = ~0;
        REG_DIC_CONTROL = 1;
    }

    REG_DIC_CLRENABLE[0] = ~0;
    for (int i = 0; i < 32; i++) REG_DIC_PRIORITY[i] = 0;
    for (int i = 0; i < 2; i++) REG_DIC_CFGREG[i] = ~0;
    REG_GIC_POI = 3;
    REG_GIC_PRIOMASK = 0xF << 4;
    REG_GIC_CONTROL = 1;

    do {
        irq_s = REG_GIC_PENDING;
        REG_GIC_IRQEND = irq_s;
    } while(irq_s != IRQ_SPURIOUS);
}

void IRQ_Register(u32 irq, ISR Handler, u32 core)
{
    assert(irq < IRQ_COUNT);
    assert(core < 2);

    if (irq < 32) IRQ_LocalHandlers[core][irq] = Handler;
    else IRQ_GlobalHandlers[irq - 32] = Handler;

    REG_DIC_CLRPENDING[irq >> 5] |= BIT(irq & 0x1F);
    REG_DIC_SETENABLE[irq >> 5]  |= BIT(irq & 0x1F);
    REG_DIC_TARGETPROC[irq] = core + 1;
}

void IRQ_Disable(u32 irq, u32 core)
{
    assert(irq < IRQ_COUNT);
    assert(core < 2);

    if (irq < 32) IRQ_LocalHandlers[core][irq] = NULL;
    else IRQ_GlobalHandlers[irq - 32] = NULL;

    REG_DIC_CLRPENDING[irq >> 5] |= BIT(irq & 0x1F);
    REG_DIC_CLRENABLE[irq >> 5]  |= BIT(irq & 0x1F);
    REG_DIC_TARGETPROC[irq] = 0;
}
