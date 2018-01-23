#include <common.h>
#include <pxi.h>

void pxi_reset(void)
{
    *PXI_SYNC = 0;
    *PXI_CNT = PXI_SEND_FIFO_CLEAR | PXI_FIFO_ENABLE;
    for (int i = 0; i < 16; i++)
        *PXI_RECV;

    *PXI_CNT = 0;
    *PXI_CNT = PXI_FIFO_ERROR_ACK | PXI_FIFO_ENABLE;

    pxi_set_remote(PXI_INITIAL_SYNC_SEND);
    while(pxi_get_remote() != PXI_INITIAL_SYNC_RECV);

    *PXI_SYNC_CNT = PXI_SYNC_IRQ_ENABLE;
    return;
}

u8 pxi_get_remote(void)
{
    return *PXI_SYNC_RECV;
}

void pxi_set_remote(u8 data)
{
    *PXI_SYNC_SEND = data;
    return;
}

u32 pxi_recv_FIFO(void)
{
    while(*PXI_CNT & PXI_RECV_FIFO_EMPTY);
    return *PXI_RECV;
}

void pxi_send_FIFO(u32 data)
{
    while(*PXI_CNT & PXI_SEND_FIFO_FULL);
    *PXI_SEND = data;
    return;
}

int pxi_recv_FIFO_data(u32 *data, u32 datac)
{
    u32 c;

    // TODO: bugcheck
    if (datac > PXI_FIFO_WIDTH) return -1;

    for (c = 0; c < datac && !(*PXI_CNT & PXI_RECV_FIFO_EMPTY); c++)
        data[c] = pxi_recv_FIFO();
    return c;
}

int pxi_send_FIFO_data(const u32 *data, u32 datac)
{
    u32 c;

    // TODO: bugcheck
    if (datac > PXI_FIFO_WIDTH) return -1;

    for (c = 0; c < datac && !(*PXI_CNT & PXI_SEND_FIFO_FULL); c++)
        pxi_send_FIFO(data[c]);

    return c;
}

void pxi_sync(void)
{
    *PXI_SYNC_CNT |= PXI_TRIGGER_IRQ;
    return;
}

void pxi_set_IRQ(bool enable) {
    if (enable)
        *PXI_SYNC_CNT |= PXI_SYNC_IRQ_ENABLE;
    else
        *PXI_SYNC_CNT &= ~PXI_SYNC_IRQ_ENABLE;
    return;
}
