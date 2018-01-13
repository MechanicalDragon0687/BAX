#include <common.h>
#include <pxi.h>

NOINLINE int pxicmd_send(u32 cmd_id, const u32 *args, u32 argc)
{
	int ret;

	// TODO: bugcheck
	if (argc > PXICMD_MAX_ARGC || cmd_id > PXICMD_MAX_CMD_ID) return -1;

	// TODO: bugcheck
	if ((u32)pxi_send_FIFO_data(args, argc) != argc) { ; }

	pxi_set_remote(PXICMD_MAKE(cmd_id, argc));
	pxi_sync();

	// wait for reply, possibly add a timeout?
	while(*PXI_CNT & PXI_RECV_FIFO_EMPTY);
	ret = pxi_recv_FIFO();

	pxi_set_remote(0);
	return ret;
}

NOINLINE u8 pxicmd_recv(u32 *args)
{
	u8 cmd = pxi_get_remote();
	pxi_recv_FIFO_data(args, PXICMD_ARGC(cmd));
	return cmd;
}

NOINLINE void pxicmd_reply(int resp)
{
	pxi_send_FIFO(resp);
	pxi_set_remote(0);
	return;
}
