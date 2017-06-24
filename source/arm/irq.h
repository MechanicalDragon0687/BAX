#pragma once

#include <common.h>
#include <arm/arm.h>

typedef void (*irq_handler)(uint32_t);

#define IRQ_IE ((uint32_t*)(0x10001000))
#define IRQ_IF ((uint32_t*)(0x10001004))

static inline void irq_disable(void)
{
    write_cpsr(read_cpsr() | (FIQ_BIT | IRQ_BIT));
    return;
}

static inline void irq_enable(void)
{
    write_cpsr(read_cpsr() & ~(FIQ_BIT | IRQ_BIT));
    return;
}

void irq_ack(const unsigned int id);
void irq_register(const unsigned int id, irq_handler func);
void irq_deregister(const unsigned int id);
void irq_reset(void);
