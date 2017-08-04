#include <common.h>

#include <cpu.h>
#include <gic.h>
#include <gpu.h>
#include <gfx.h>
#include <pxi.h>
#include <vq.h>

volatile vq wq, fq;

void pxi_sync_handler(u32 xrq)
{
    gpu_memoryfill(VRAM_START, (GFX_GREEN) | (GFX_GREEN<<16), VRAM_END, PSC_32BIT);
    return;
}

void main(void)
{
    u32 *framebuffers = (u32*)FRAMEBUFFERS;
    memcpy(framebuffers, def_fbs, sizeof(def_fbs));

    gpu_init();

    vq_init(&wq);
    vq_init(&fq);

    gic_configure(IRQ_PXI_SYNC, pxi_sync_handler);
    pxi_enable_irq();

    cpu_EnableIRQ(0);
    while(1);
}
