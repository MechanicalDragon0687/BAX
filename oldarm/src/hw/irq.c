#include <common.h>
#include <cpu.h>
#include <interrupt.h>

#include "hw/irq.h"

#define REG_IRQ_IE (*((vu32*)0x10001000))
#define REG_IRQ_IF (*((vu32*)0x10001004))

ISR IRQ_Handlers[IRQ_COUNT];

void IRQ_Reset(void)
{
    // Clear the interrupt handler table
    memset(IRQ_Handlers, 0, sizeof(IRQ_Handlers));

    // Disable all interrupts, acknowledge all pending interrupts
    REG_IRQ_IE = 0;
    do {
        REG_IRQ_IF = 0xFFFFFFFFUL;
    } while(REG_IRQ_IF != 0);
}

void IRQ_Register(u32 irq, ISR Handler)
{
    CritStat s;
    assert(irq < IRQ_COUNT);

    s = CPU_EnterCritical();
    IRQ_Handlers[irq] = Handler;
    REG_IRQ_IF  = BIT(irq);
    REG_IRQ_IE |= BIT(irq);
    CPU_LeaveCritical(s);
}

void IRQ_Disable(u32 irq)
{
    CritStat s;
    assert(irq < IRQ_COUNT);

    s = CPU_EnterCritical();
    IRQ_Handlers[irq] = NULL;
    REG_IRQ_IF  = BIT(irq);
    REG_IRQ_IE &= ~BIT(irq);
    CPU_LeaveCritical(s);
}
