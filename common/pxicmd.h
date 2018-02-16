#ifndef PXICMD_H
#define PXICMD_H

#include <types.h>
#include <pxi.h>

#define PXICMD_MAX_ARGC            (15)
#define PXICMD_MAX_CMD_ID          (255)

// ARM11 -> ARM9
#define PXICMD_ARM9_SDMMC_INIT     (0)
#define PXICMD_ARM9_SD_READSECTORS (1)
#define PXICMD_ARM9_FIRMVERIFY     (2)
#define PXICMD_ARM9_FIRMBOOT       (3)
#define PXICMD_ARM9_HALT           (4)
#define PXICMD_ARM9_BOOTENV        (5)
#define PXICMD_ARM9_RANDOM         (6)

// ARM9 -> ARM11
#define PXICMD_ARM11_PANIC         (0)


#ifndef __ASSEMBLER__

void PXICMD_SendAsync(u32 cmd, const u32 *a, int c);
int  PXICMD_SendWait(void);
int  PXICMD_Send(u32 cmd, const u32 *a, int c);
int  PXICMD_Recv(u32 *a, int *c);
void PXICMD_Reply(int r);

#endif // __ASSEMBLER__

#endif // PXICMD_H
