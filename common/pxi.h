#ifndef PXI_H
#define PXI_H

#include <types.h>

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

#endif // ARM9


#define REG_PXI_CNT        (*((vu16*)(PXI_BASE + 0x04)))

#define REG_PXI_SEND       (*((vu32*)(PXI_BASE + 0x08)))
#define REG_PXI_RECV       (*((vu32*)(PXI_BASE + 0x0C)))

#define REG_PXI_SYNC_RECV  (*((vu8*)(PXI_BASE + 0x00)))
#define REG_PXI_SYNC_SEND  (*((vu8*)(PXI_BASE + 0x01)))
#define REG_PXI_SYNC_CNT   (*((vu8*)(PXI_BASE + 0x03)))


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

void PXI_Reset(void);

static inline bool PXI_SendFIFOFull(void) {
    return (REG_PXI_CNT & PXI_SEND_FIFO_FULL);
}

static inline bool PXI_SendFIFOEmpty(void) {
    return (REG_PXI_CNT & PXI_SEND_FIFO_EMPTY);
}

static inline bool PXI_RecvFIFOFull(void) {
    return (REG_PXI_CNT & PXI_RECV_FIFO_FULL);
}

static inline bool PXI_RecvFIFOEmpty(void) {
    return (REG_PXI_CNT & PXI_RECV_FIFO_EMPTY);
}

static inline void PXI_Sync(void) {
    REG_PXI_SYNC_CNT |= PXI_TRIGGER_IRQ;
}

static inline void PXI_SyncIRQSet(bool enable) {
    if (enable == true) REG_PXI_SYNC_CNT |= PXI_SYNC_IRQ_ENABLE;
    else REG_PXI_SYNC_CNT &= ~PXI_SYNC_IRQ_ENABLE;
}

static inline u8 PXI_RecvSync(void) {
    return REG_PXI_SYNC_RECV;
}

static inline void PXI_SendSync(u8 b) {
    REG_PXI_SYNC_SEND = b;
}

static inline u32 PXI_Recv(void) {
    while(PXI_RecvFIFOEmpty());
    return REG_PXI_RECV;
}

static inline void PXI_Send(u32 w) {
    while(PXI_SendFIFOFull());
    REG_PXI_SEND = w;
}

int  PXI_RecvData(u32 *w, int wc);
int  PXI_SendData(const u32 *w, int wc);
#endif // __ASSMBLER__

#endif // PXI_H
