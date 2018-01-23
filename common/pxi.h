#pragma once

#include <common.h>

#ifdef ARM9
#define PXI_BASE (0x10008000)
#define PXI_TRIGGER_IRQ (BIT(5))
#define PXI_INITIAL_SYNC_SEND (0)
#define PXI_INITIAL_SYNC_RECV (1)
#else
#define PXI_BASE (0x10163000)
#define PXI_TRIGGER_IRQ (BIT(6))
#define PXI_INITIAL_SYNC_SEND (1)
#define PXI_INITIAL_SYNC_RECV (0)
#endif

#define PXI_SYNC ((vu32*)(PXI_BASE + 0x00))
#define PXI_CNT  ((vu16*)(PXI_BASE + 0x04))
#define PXI_SEND ((vu32*)(PXI_BASE + 0x08))
#define PXI_RECV ((vu32*)(PXI_BASE + 0x0C))

#define PXI_SYNC_RECV  ((vu8*)(PXI_BASE + 0x00))
#define PXI_SYNC_SEND  ((vu8*)(PXI_BASE + 0x01))
#define PXI_SYNC_CNT   ((vu8*)(PXI_BASE + 0x03))

#define PXI_SEND_FIFO_EMPTY      (BIT(0))
#define PXI_SEND_FIFO_FULL       (BIT(1))
#define PXI_SEND_FIFO_EMPTY_IRQ  (BIT(2))
#define PXI_SEND_FIFO_CLEAR      (BIT(3))
#define PXI_RECV_FIFO_EMPTY      (BIT(8))
#define PXI_RECV_FIFO_FULL       (BIT(9))
#define PXI_RECV_FIFO_NEMPTY_IRQ (BIT(10))
#define PXI_FIFO_ERROR_ACK       (BIT(14))
#define PXI_FIFO_ENABLE          (BIT(15))

#define PXI_SYNC_IRQ_ENABLE      (BIT(7))

#define PXI_FIFO_WIDTH           (16)

#ifndef __ASSEMBLER__
void pxi_reset(void);
u8   pxi_get_remote(void);
void pxi_set_remote(u8 data);
u32  pxi_recv_FIFO(void);
void pxi_send_FIFO(u32 data);
int  pxi_recv_FIFO_data(u32 *data, u32 datac);
int  pxi_send_FIFO_data(const u32 *data, u32 datac);
void pxi_sync(void);
void pxi_set_IRQ(bool enable);
#endif
