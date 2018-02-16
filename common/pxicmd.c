#include <common.h>

#include <pxi.h>
#include <pxicmd.h>

void PXICMD_SendAsync(u32 cmd, const u32 *a, int c)
{
    while(!PXI_SendFIFOEmpty());
    PXI_SendData(a, c);
    PXI_SendSync(cmd);
    PXI_Sync();
    return;
}

int PXICMD_SendWait(void)
{
    int ret = PXI_Recv();
    PXI_SendSync(0);
    return ret;    
}

int PXICMD_Send(u32 cmd, const u32 *a, int c)
{
    PXICMD_SendAsync(cmd, a, c);
    return PXICMD_SendWait();
}

int PXICMD_Recv(u32 *a, int *c)
{
    int count, cmd;
    cmd = PXI_RecvSync();
    count = PXI_RecvData(a, PXICMD_MAX_ARGC);
    if (c) *c = count;
    return cmd;
}

void PXICMD_Reply(int r) {
    PXI_Send(r);
    PXI_SendSync(0);
}
