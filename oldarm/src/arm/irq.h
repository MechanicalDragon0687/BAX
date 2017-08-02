#pragma once

#include <common.h>
#include <cpu.h>

typedef void (*irq_handler)(u32);

#define REG_IRQ_IE ((vu32*)(0x10001000))
#define REG_IRQ_IF ((vu32*)(0x10001004))

#define IRQ_COUNT    (29)

static inline void irq_ack(unsigned int id)
{
    *REG_IRQ_IF = BIT(id);
    return;
}

void irq_register(unsigned int id, irq_handler func);
void irq_deregister(unsigned int id);
void irq_reset(void);
