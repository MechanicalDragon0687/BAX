#include <common.h>
#include <cache.h>
#include <pxi.h>
#include <cpu.h>
#include <irq.h>

#define PXICMD_CODE
#include <pxicmd.h>

#include "anim.h"
#include "console.h"
#include "hw/int.h"
#include "hw/timer.h"

#include "lib/ff/ff.h"

void pxi_handler(u32 irqn)
{
	u8 cmd;
	u32 pxia[PXICMD_MAX_ARGC];
	int resp = 0;

	cmd = pxicmd_recv(pxia);
	switch(PXICMD_CMDID(cmd))
	{
		default:
		// TODO: bugcheck
			break;
	}

	pxicmd_reply(resp);
	return;
}

void main(void)
{
	FATFS main_fs;
	FIL main_bax;
	FRESULT fres;
	size_t br, timer;
	void *dest;

	console_reset();
	console_puts("hello from ARM11\n");
	irq_register(IRQ_PXI_SYNC, pxi_handler);
	pxi_reset();

	fres = f_mount(&main_fs, "sdmc:", 1);
	console_puts("f_mount = ");
	console_putd(fres);
	console_putc('\n');

	fres = f_open(&main_bax, "sdmc:/main.bax", FA_READ);
	console_puts("f_open = ");
	console_putd(fres);
	console_putc('\n');

	// TODO: bugcheck
	if (fres != FR_OK)
		while(1) _wfi();

	console_puts("f_size = ");
	console_putd(f_size(&main_bax));
	console_putc('\n');

	dest = malloc(f_size(&main_bax));
	if (dest == NULL)
		while(1) _wfi();

	timer_start(~0, false, false);

	fres = f_read(&main_bax, dest, f_size(&main_bax), &br);
	timer = timer_ticks();
	timer_stop();

	console_puts("f_read = ");
	console_putd(fres);
	console_putc('\n');

	console_puts("br = ");
	console_putd(br);
	console_putc('\n');

	console_puts("ticks = ");
	console_puth(timer);
	console_putc('\n');

	console_puts("ms = ");
	console_putd((u32)timer_ticks_to_ms(0xFFFFFFFF - timer));
	console_putc('\n');

	f_close(&main_bax);

	int res = anim_validate((anim_t*)dest, br);
	console_puts("anim_validate = ");
	console_putd(res);
	console_putc('\n');
	if (res != ANIM_OK)
	{
		console_puts("Bad anim: ");
		console_puts(anim_get_error(res));
		while(1) _wfi();
	}

	anim_play((anim_t*)dest);
	while(1) _wfi();
}
