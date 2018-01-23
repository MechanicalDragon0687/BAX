#pragma once

#include <common.h>
#include <cpu.h>

#define PXICMD_MAX_ARGC    (15)
#define PXICMD_MAX_CMD_ID  (255)

#define PXICMD_ARM9_SDMMC_INIT     (0)
#define PXICMD_ARM9_SD_READSECTORS (1)
#define PXICMD_ARM9_FIRMVERIFY     (2)
#define PXICMD_ARM9_FIRMBOOT       (3)
#define PXICMD_ARM9_HALT           (4)
#define PXICMD_ARM9_BOOTENV        (5)
#define PXICMD_ARM9_RANDOM         (6)

#define PXICMD_ARM11_PANIC         (0)

#ifndef __ASSEMBLER__
void pxicmd_send_async(u32 cmd_id, const u32 *args, u32 argc);
int  pxicmd_send_finish(void);
int  pxicmd_send(u32 cmd_id, const u32 *args, u32 argc);
u8   pxicmd_recv(u32 *args, u32 *argc);
void pxicmd_reply(int resp);
#endif
