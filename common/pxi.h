#pragma once
#include <common.h>

#ifdef ARM9
#define PXI_BASE (0x10008000)
#define IRQ_PXI_SYNC (12)
#else
#define PXI_BASE (0x10163000)
#define IRQ_PXI_SYNC (80)
#endif

#define PXI_CMD_PLAYANIM (0xFE)
#define PXI_CMD_FIRMBOOT (0xFD)

#define PXI_SYNC_RECV ((vu8*)(PXI_BASE + 0x00))
#define PXI_SYNC_SEND ((vu8*)(PXI_BASE + 0x01))
#define PXI_SYNC_IRQ  ((vu8*)(PXI_BASE + 0x03))
#define PXI_CNT       ((vu16*)(PXI_BASE + 0x04))
#define PXI_SEND      ((vu32*)(PXI_BASE + 0x08))
#define PXI_RECV      ((vu32*)(PXI_BASE + 0x0C))

#define PXI_CNT_SEND_FIFO_EMPTY       (BIT(0))
#define PXI_CNT_SEND_FIFO_FULL        (BIT(1))
#define PXI_CNT_SEND_FIFO_EMPTY_IRQ   (BIT(2))
#define PXI_CNT_SEND_FIFO_FLUSH       (BIT(3))
#define PXI_CNT_RECV_FIFO_EMPTY       (BIT(8))
#define PXI_CNT_RECV_FIFO_FULL        (BIT(9))
#define PXI_CNT_RECV_FIFO_NEMPTY_IRQ  (BIT(10))
#define PXI_CNT_ERROR_ACK             (BIT(14))
#define PXI_CNT_ENABLE_FIFO           (BIT(15))

#define PXI_SYNC_TRIGGER_MPCORE (BIT(5))
#define PXI_SYNC_TRIGGER_OLDARM (BIT(6))
#define PXI_SYNC_ENABLE_IRQ     (BIT(7))

static inline void pxi_send_rem(u8 msg)
{
    *PXI_SYNC_SEND = msg;
}

static inline u8 pxi_recv_rem(void)
{
    return *PXI_SYNC_RECV;
}

static inline void pxi_enable_irq(void)
{
    *PXI_SYNC_IRQ = PXI_SYNC_ENABLE_IRQ;
}

static inline void pxi_disable_irq(void)
{
    *PXI_SYNC_IRQ = 0;
}

static inline void pxi_sync(void)
{
    #ifdef ARM9
    *PXI_SYNC_IRQ |= PXI_SYNC_TRIGGER_MPCORE;
    #else
    *PXI_SYNC_IRQ |= PXI_SYNC_TRIGGER_OLDARM;
    #endif
}

static inline void pxi_reset(void)
{
    *PXI_SYNC_SEND = 0;
    *PXI_SYNC_IRQ = 0;
    *PXI_CNT = PXI_CNT_SEND_FIFO_FLUSH | PXI_CNT_ENABLE_FIFO;
}

static inline void pxi_putw(u32 w)
{
    while(*PXI_CNT & PXI_CNT_SEND_FIFO_FULL);
    do {
        *PXI_SEND = w;
    } while(*PXI_CNT & PXI_CNT_ERROR_ACK);
    return;
}

static inline u32 pxi_getw(void)
{
    u32 ret;
    while(*PXI_CNT & PXI_CNT_RECV_FIFO_EMPTY);
    do {
        ret = *PXI_RECV;
    } while(*PXI_CNT & PXI_CNT_ERROR_ACK);
    return ret;
}



