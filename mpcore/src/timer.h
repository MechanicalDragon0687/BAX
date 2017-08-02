#pragma once
#include <common.h>
#include <cpu.h>

#define TIMER_BASE (0x17E00600)

#define TIMER_COUNTER ((vu32*)(TIMER_BASE + 0x04))
#define TIMER_CONTROL ((vu32*)(TIMER_BASE + 0x08))
#define TIMER_UDFLOW  ((vu32*)(TIMER_BASE + 0x0C))

#define TIMER_ENABLE     (BIT(0))
#define TIMER_AUTORELOAD (BIT(1))
#define TIMER_ENABLE_IRQ (BIT(2))
