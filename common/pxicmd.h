#pragma once

#include <common.h>
#include <cpu.h>

#define PXICMD_MAX_ARGC    (15)
#define PXICMD_MAX_CMD_ID  (255)

enum
{
    PXICMD_ARM9_SYSINFO = 0,
    PXICMD_ARM9_SDMMC_INIT,
    PXICMD_ARM9_SD_READSECTORS,
    PXICMD_ARM9_FIRMVERIFY,
    PXICMD_ARM9_FIRMBOOT
};

enum
{
    PXICMD_ARM11_NONE_YET
};

// Send a PXI command and it's arguments
int pxicmd_send(u32 cmd_id, const u32 *args, u32 argc);

// Receive a PXI command and it's arguments
u8  pxicmd_recv(u32 *args, u32 *argc);

// Reply to a PXI command
void pxicmd_reply(int resp);

#ifdef PXICMD_CODE
#include <pxicmd.c>
#endif
