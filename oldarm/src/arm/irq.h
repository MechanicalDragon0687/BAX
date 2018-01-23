#pragma once

#include <common.h>
#include <interrupt.h>

#define IRQ_IE    ((vu32*)0x10001000)
#define IRQ_IF    ((vu32*)0x10001004)

typedef void (*isr_t)(u32 irqn);

// Reinitialize the interrupt controller
void irq_reset(void);

// Enable the interrupt and register a handler in case it's triggered
void irq_register(u32 irqn, isr_t handler);

// Disable interrupt
void irq_deregister(u32 irqn);
