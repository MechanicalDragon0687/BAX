#ifndef IRQ_H
#define IRQ_H

#include <common.h>
#include <interrupt.h>

#define REG_GIC_BASE  (0x17E00100) // MPCore PMR
#define REG_DIC_BASE  (0x17E01000)

#define REG_GIC_CONTROL    (*(vu32*)(REG_GIC_BASE + 0x00))
#define REG_GIC_PRIOMASK   (*(vu32*)(REG_GIC_BASE + 0x04))
#define REG_GIC_POI        (*(vu32*)(REG_GIC_BASE + 0x08))
#define REG_GIC_IRQACK     (*(vu32*)(REG_GIC_BASE + 0x0C))
#define REG_GIC_IRQEND     (*(vu32*)(REG_GIC_BASE + 0x10))
#define REG_GIC_LASTPRIO   (*(vu32*)(REG_GIC_BASE + 0x14))
#define REG_GIC_PENDING    (*(vu32*)(REG_GIC_BASE + 0x18))

#define REG_DIC_CONTROL    (*(vu32*)(REG_DIC_BASE + 0x00))
#define REG_DIC_SETENABLE  ((vu32*)(REG_DIC_BASE + 0x100))
#define REG_DIC_CLRENABLE  ((vu32*)(REG_DIC_BASE + 0x180))
#define REG_DIC_SETPENDING ((vu32*)(REG_DIC_BASE + 0x200))
#define REG_DIC_CLRPENDING ((vu32*)(REG_DIC_BASE + 0x280))
#define REG_DIC_PRIORITY   ((vu32*)(REG_DIC_BASE + 0x400))
#define REG_DIC_TARGETPROC ((vu8*) (REG_DIC_BASE + 0x800))
#define REG_DIC_CFGREG     ((vu32*)(REG_DIC_BASE + 0xC00))

typedef void (*ISR)(u32 irqn);

void IRQ_Reset(void);

void IRQ_Register(u32 irq, ISR Handler, u32 core);
void IRQ_Disable(u32 irq, u32 core);

#endif
