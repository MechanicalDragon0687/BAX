#include <common.h>
#include <cpu.h>
#include <pxicmd.h>

#include "hw/irq.h"
#include "hw/sdmmc.h"

#include "lib/firm/firm.h"

static FIRM *volatile firm = NULL;
static char firm_path[256] = {0};
void PXICMD_Handler(u32 irqn)
{
    u32 pxia[PXICMD_MAX_ARGC];
    int resp = 0, pxic;
    u8 cmd = PXICMD_Recv(pxia, &pxic);

    switch(cmd) {
        case PXICMD_ARM9_SDMMC_INIT:
        {
            sdmmc_init();
            resp = SD_Init();
            break;
        }

        case PXICMD_ARM9_SD_READSECTORS:
        {
            resp = sdmmc_sdcard_readsectors(pxia[0], pxia[1], (u8*)pxia[2]);
            break;
        }

        case PXICMD_ARM9_FIRMVERIFY:
        {
            resp = FIRM_Validate((FIRM*)pxia[0], pxia[1]);
            break;
        }

        case PXICMD_ARM9_FIRMBOOT:
        {
            firm = (FIRM*)pxia[0];
            strncpy(firm_path, (const char*)pxia[1], sizeof(firm_path) - 1);
            firm_path[sizeof(firm_path) - 1] = '\0';
            break;
        }

        case PXICMD_ARM9_HALT:
        {
            while(1) CPU_WFI();
        }

        case PXICMD_ARM9_BOOTENV:
        {
            resp = *(vu32*)0x10010000;
            break;
        }

        case PXICMD_ARM9_RANDOM:
        {
            resp = *(vu32*)0x10011000;
            break;
        }

        default:
            assert(0 != 0); // NO MORE GAMES
    }

    PXICMD_Reply(resp);
    return;
}

void main(void)
{
    while(firm == NULL) CPU_WFI();
    FIRM_Boot(firm, firm_path);    
}
