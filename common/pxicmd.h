#pragma once

#include <common.h>

/*
 PXICMD layout
 bit 0-3: command id
 bit 4-7: argument count
*/

#define PXICMD_MAX_ARGC    (15)
#define PXICMD_MAX_CMD_ID  (15)

#define PXICMD_MAKE(cmd,c) ((u8)((c) << 4 | (cmd)))

#define PXICMD_CMDID(x) ((x) & 15)
#define PXICMD_ARGC(x)  ((x) >> 4)

enum
{
	PXICMD_ARM9_SYSINFO = 0,
	PXICMD_ARM9_SDMMC_INIT,
	PXICMD_ARM9_SD_READSECTORS,
	PXICMD_ARM9_BOOTFIRM
};

enum
{
	PXICMD_ARM11_PANIC  = 0
};

// Send a PXI command and it's arguments
int pxicmd_send(u32 cmd_id, const u32 *args, u32 argc);

// Receive a PXI command and it's arguments
u8  pxicmd_recv(u32 *args);

// Reply to a PXI command
void pxicmd_reply(int resp);

#ifdef PXICMD_CODE
#include <pxicmd.c>
#endif
