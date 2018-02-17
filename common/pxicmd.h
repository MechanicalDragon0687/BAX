#ifndef PXICMD_H
#define PXICMD_H

#include <types.h>
#include <pxi.h>

#define PXICMD_MAX_ARGC            (16)
#define PXICMD_MAX_CMD_ID          (255)

// ARM11 -> ARM9
#define PXICMD_ARM9_SDMMC_INIT     (1)
#define PXICMD_ARM9_SD_READSECTORS (2)
#define PXICMD_ARM9_FIRMVERIFY     (3)
#define PXICMD_ARM9_FIRMBOOT       (4)
#define PXICMD_ARM9_HALT           (5)
#define PXICMD_ARM9_BOOTENV        (6)
#define PXICMD_ARM9_RANDOM         (7)

// ARM9 -> ARM11
#define PXICMD_ARM11_PANIC         (1)


#ifndef __ASSEMBLER__

void PXICMD_SendAsync(u32 cmd, const u32 *a, int c);
int  PXICMD_SendWait(void);
int  PXICMD_Send(u32 cmd, const u32 *a, int c);
int  PXICMD_Recv(u32 *a, int *c);
void PXICMD_Reply(int r);

#endif // __ASSEMBLER__

#endif // PXICMD_H
