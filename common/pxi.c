#include <common.h>
#include <pxi.h>

void PXI_Reset(void)
{
    // Disable SYNC interrupt
    REG_PXI_SYNC_CNT = 0;

    // Enable and clear send FIFO
    REG_PXI_CNT = PXI_SEND_FIFO_CLEAR | PXI_FIFO_ENABLE;

    // Clear recv FIFO
    for (int i = 0; i < PXI_FIFO_WIDTH; i++)
        REG_PXI_RECV;

    // Reset PXI
    REG_PXI_CNT = 0;
    REG_PXI_CNT = PXI_FIFO_ERROR_ACK | PXI_FIFO_ENABLE;

    // Synchronize the two processors
    PXI_SendSync(PXI_INITIAL_SYNC_SEND);
    while(PXI_RecvSync() != PXI_INITIAL_SYNC_RECV);

    // Enable SYNC interrupt by default
    REG_PXI_SYNC_CNT = PXI_SYNC_IRQ_ENABLE;
    return;
}

int PXI_RecvData(u32 *w, int wc)
{
    int i = 0;
    while (i < wc && !PXI_RecvFIFOEmpty())
        w[i++] = PXI_Recv();
    return i;
}

int PXI_SendData(const u32 *w, int wc)
{
    int i = 0;
    while (i < wc && !PXI_SendFIFOFull())
        PXI_Send(w[i++]);
    return i;
}
