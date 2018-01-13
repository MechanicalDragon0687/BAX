#pragma once

// ARM9 hardware interrupts
enum
{
	IRQ_DMA0          = 0,
	IRQ_DMA1          = 1,
	IRQ_DMA2          = 2,
	IRQ_DMA3          = 3,
	IRQ_DMA4          = 4,
	IRQ_DMA5          = 5,
	IRQ_DMA6          = 6,
	IRQ_DMA7          = 7,
	IRQ_TIMER0        = 8,
	IRQ_TIMER1        = 9,
	IRQ_TIMER2        = 10,
	IRQ_TIMER3        = 11,
	IRQ_PXI_SYNC      = 12,
	IRQ_PXI_NOT_FULL  = 13,
	IRQ_PXI_NOT_EMPTY = 14,
	IRQ_AES           = 15,
	IRQ_SDIO1         = 16,
	IRQ_SDIO1_ASYNC   = 17,
	IRQ_SDIO3         = 18,
	IRQ_SDIO3_ASYNC   = 19,
	IRQ_DEBUG_RECV    = 20,
	IRQ_DEBUG_SEND    = 21,
	IRQ_RSA           = 22,
	IRQ_CTRCARD1      = 23,
	IRQ_CTRCARD2      = 24,
	IRQ_CGC           = 25,
	IRQ_CGC_DET       = 26,
	IRQ_DMAC2         = 27,
	IRQ_DMAC2_ABT     = 28
};
