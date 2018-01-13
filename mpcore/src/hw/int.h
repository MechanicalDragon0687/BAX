#pragma once

// Known ARM11 hardware interrupts
enum
{
	IRQ_TIMER         = 0x1D,
	IRQ_WATCHDOG      = 0x1E,
	IRQ_PSC0          = 0x28,
	IRQ_PSC1          = 0x29,
	IRQ_VBLANK0       = 0x2A,
	IRQ_VBLANK1       = 0x2B,
	IRQ_PXI_SYNC      = 0x50,
	IRQ_PXI_NOT_FULL  = 0x52,
	IRQ_PXI_NOT_EMPTY = 0x53,
	IRQ_PDN           = 0x58,
	IRQ_MCU_HID       = 0x71
};
