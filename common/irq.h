#pragma once

#ifdef ARM9

#define IRQ_BASE  ((vu32*)0x10001000)
#define IRQ_COUNT (29)

#else

#define GIC_BASE  (0x17E00100) // MPCore PMR
#define DIC_BASE  (0x17E01000)

#define GIC_CONTROL    ((vu32*)(GIC_BASE + 0x00))
#define GIC_PRIOMASK   ((vu32*)(GIC_BASE + 0x04))
#define GIC_POI        ((vu32*)(GIC_BASE + 0x08))
#define GIC_IRQACK     ((vu32*)(GIC_BASE + 0x0C))
#define GIC_IRQEND     ((vu32*)(GIC_BASE + 0x10))
#define GIC_LASTPRIO   ((vu32*)(GIC_BASE + 0x14))
#define GIC_PENDING    ((vu32*)(GIC_BASE + 0x18))

#define DIC_CONTROL    ((vu32*)(DIC_BASE + 0x000))
#define DIC_SETENABLE  ((vu32*)(DIC_BASE + 0x100))
#define DIC_CLRENABLE  ((vu32*)(DIC_BASE + 0x180))
#define DIC_SETPENDING ((vu32*)(DIC_BASE + 0x200))
#define DIC_CLRPENDING ((vu32*)(DIC_BASE + 0x280))
#define DIC_PRIORITY   ((vu32*)(DIC_BASE + 0x400))
#define DIC_TARGETPROC ((vu8*) (DIC_BASE + 0x800))
#define DIC_CFGREG     ((vu32*)(DIC_BASE + 0xC00))

#define IRQ_COUNT (128)
#endif

typedef void (*isr_t)(u32 irqn);

#ifdef IRQ_CODE
#include <irq.c>
#endif

// Reinitialize the interrupt controller
void irq_reset(void);

// Enable the interrupt and register a handler in case it's triggered
void irq_register(u32 irqn, isr_t handler);

// Disable interrupt
void irq_deregister(u32 irqn);

// Acknowledge interrupt
void irq_ack(u32 irqn);

// Get highest priority pending interrupt
u32  irq_pending(void);

// Get interrupt handler
isr_t irq_handler(u32 irqn);

// Check interrupt status
bool irq_enabled(u32 irqn);
