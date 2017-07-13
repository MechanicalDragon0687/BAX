#pragma once

#include <common.h>
#include <arm/cpu.h>

typedef void (*irq_handler)(uint32_t);

#define REG_IRQ_IE ((uint32_t*)(0x10001000))
#define REG_IRQ_IF ((uint32_t*)(0x10001004))

#define IRQ_TIMER(n) (8 + (n))

#define ENTER_CRITICAL(x) do { (x) = irq_kill(); } while(0)
#define LEAVE_CRITICAL(x) do { irq_restore((x)); } while(0)

static inline uint32_t irq_kill(void)
{
    uint32_t cpsr;
    cpsr = read_cpsr();
    write_cpsr(cpsr | (SR_IRQ_BIT | SR_FIQ_BIT));
    return cpsr & (SR_IRQ_BIT | SR_FIQ_BIT);
}

static inline void irq_restore(uint32_t ss)
{
    uint32_t cpsr;
    cpsr = read_cpsr() & ~(SR_IRQ_BIT | SR_FIQ_BIT);
    write_cpsr(cpsr | ss);
    return;
}

static inline void irq_ack(const unsigned int id)
{
    *REG_IRQ_IF = BIT(id);
    return;
}

void irq_register(const unsigned int id, irq_handler func);
void irq_deregister(const unsigned int id);
void irq_reset(void);
