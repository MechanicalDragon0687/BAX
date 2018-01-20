#include <common.h>
#include <cache.h>
#include <cpu.h>
#include <sys.h>

#define PXI_CODE
#include <pxi.h>

#define PXICMD_CODE
#include <pxicmd.h>

#include "arm/irq.h"
#include "hw/int.h"
#include "hw/sdmmc.h"

#include "lib/firm/firm.h"

void fill_sysinfo(sysinfo_t *info)
{
    info->bootenv = *(u32*)0x10010000;
    info->rndseed = *(u32*)0x10011000;
    return;
}

static firm_t *volatile firm = NULL;
static char firm_path[256] = {0};
void pxi_handler(u32 irqn)
{
    u8 cmd;
    u32 pxia[PXICMD_MAX_ARGC], pxic;
    int resp = 0;

    cmd = pxicmd_recv(pxia, &pxic);
    switch(cmd)
    {
        case PXICMD_ARM9_SYSINFO:
        {
            fill_sysinfo((sysinfo_t*)pxia[0]);
            break;
        }

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
            resp = firm_validate((firm_t*)pxia[0], pxia[1]);
            break;
        }

        case PXICMD_ARM9_FIRMBOOT:
        {
            firm = (firm_t*)pxia[0];
            strncpy(firm_path, (const char*)pxia[1], 255);
            break;
        }

        default:
        // TODO: bugcheck
            break;
    }

    pxicmd_reply(resp);
    return;
}

void main(u32 *args)
{
    irq_register(IRQ_PXI_SYNC, pxi_handler);
    pxi_reset();

    while(firm == NULL) _wfi();
    firm_boot(firm, firm_path);    
}
