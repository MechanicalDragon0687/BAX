#ifndef IRQ_H
#define IRQ_H

#include <types.h>

typedef void (*ISR)(u32 irqn);

void IRQ_Reset(void);

void IRQ_Register(u32 irq, ISR Handler);
void IRQ_Disable(u32 irq);

#endif // IRQ_H
