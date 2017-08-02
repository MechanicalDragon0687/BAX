#pragma once
#include <common.h>

#ifdef ARM9
#define PXI_BASE (0x10008000)
#else
#define PXI_BASE (0x10163000)
#endif

typedef enum {
    CMD_SCREENINIT = 0,
    CMD_PLAYANIM = 1,
    CMD_OOB
} pxi_cmds;

#define PXI_SYNC_RECV ((u8*)(PXI_BASE + 0x00))
#define PXI_SYNC_SEND ((u8*)(PXI_BASE + 0x01))
#define PXI_SYNC_IRQ  ((u8*)(PXI_BASE + 0x03))
#define PXI_CNT  ((u16*)(PXI_BASE + 0x04))
#define PXI_SEND ((u32*)(PXI_BASE + 0x08))
#define PXI_RECV ((u32*)(PXI_BASE + 0x0C))

#define PXI_CNT_SEND_FIFO_EMPTY       (BIT(0))
#define PXI_CNT_SEND_FIFO_FULL        (BIT(1))
#define PXI_CNT_SEND_FIFO_EMPTY_IRQ   (BIT(2))
#define PXI_CNT_SEND_FIFO_FLUSH       (BIT(3))
#define PXI_CNT_RECV_FIFO_EMPTY       (BIT(8))
#define PXI_CNT_RECV_FIFO_FULL        (BIT(9))
#define PXI_CNT_RECV_FIFO_NEMPTY_IRQ  (BIT(10))
#define PXI_CNT_SEND_RECV_ERROR_ACK   (BIT(14))
#define PXI_CNT_ENABLE_SEND_RECV_FIFO (BIT(15))

#define PXI_SYNC_TRIGGER_MPCORE (BIT(5))
#define PXI_SYNC_TRIGGER_OLDARM (BIT(6))
#define PXI_SYNC_ENABLE_IRQ     (BIT(7))
