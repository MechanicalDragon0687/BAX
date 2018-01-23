#include <common.h>

#include <pxi.h>
#include <pxicmd.h>

void pxicmd_send_async(u32 cmd_id, const u32 *args, u32 argc)
{
    while(!(*PXI_CNT & PXI_SEND_FIFO_EMPTY));
    pxi_send_FIFO_data(args, argc);

    pxi_set_remote(cmd_id);
    pxi_sync();
    return;
}

int pxicmd_send_finish(void)
{
    int ret;

    // wait for reply
    while(*PXI_CNT & PXI_RECV_FIFO_EMPTY);
    ret = pxi_recv_FIFO();

    pxi_set_remote(0);
    return ret;
}

int pxicmd_send(u32 cmd_id, const u32 *args, u32 argc)
{
    pxicmd_send_async(cmd_id, args, argc);
    return pxicmd_send_finish();
}

u8 pxicmd_recv(u32 *args, u32 *argc)
{
    u32 count;
    u8 cmd = pxi_get_remote();
    count = pxi_recv_FIFO_data(args, PXICMD_MAX_ARGC);
    if (argc) *argc = count;
    return cmd;
}

void pxicmd_reply(int resp)
{
    pxi_send_FIFO(resp);
    pxi_set_remote(0);
    return;
}
