#include <common.h>
#include <cpu.h>
#include <sys.h>

#define CACHE_CODE
#include <cache.h>

#define PXI_CODE
#include <pxi.h>

#define IRQ_CODE
#include <irq.h>

#define PXICMD_CODE
#include <pxicmd.h>

#include "hw/int.h"
#include "hw/sdmmc.h"

void fill_sysinfo(sysinfo_t *info)
{
	info->sysprot = *(u8*)0x10000000;
	info->bootenv = *(u32*)0x10010000;
	info->rndseed = *(u32*)0x10011000;
	return;
}

void pxi_handler(u32 irqn)
{
	u8 cmd;
	u32 pxia[PXICMD_MAX_ARGC];
	int resp = 0;

	cmd = pxicmd_recv(pxia);
	switch(PXICMD_CMDID(cmd))
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
	while(1) _wfi();
}
