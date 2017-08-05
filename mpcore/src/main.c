#include <common.h>

#include <anim.h>
#include <bax.h>
#include <cpu.h>
#include <gic.h>
#include <gpu.h>
#include <gfx.h>
#include <pxi.h>
#include <vq.h>

volatile vq wq, fq[ANIM_TARGETS];
vu32 count;

void pxi_sync_handler(u32 xrq)
{
    u8 msg = pxi_recv_rem();

    switch(msg) {
    case PXI_CMD_FIRMBOOT:
    {
        u32 entry;
        while((entry=(*(vu32*)(0x1FFFFFFC)))==0);
        ((void (*)(void))(entry))();
        while(1);
    }
    case PXI_CMD_PLAYANIM:
    {
        void *anim;
        u32 anim_size;

        anim = (void*)pxi_getw();
        anim_size = pxi_getw();

        cpu_InvalidateDCRange((u32)anim, (u32)anim + anim_size);
        bax_cvt_wq(anim, anim_size, &wq);
        break;
    }
    default:
        break;
    }
    return;
}

void timer_handler(u32 xrq)
{
    for (int i = 0; i < ANIM_TARGETS; i++) {
        bax_stream_fq(i, &(fq[i]));
    }
    return;
}

void main(void)
{
    u32 *framebuffers = (u32*)FRAMEBUFFERS;
    memcpy(framebuffers, def_fbs, sizeof(def_fbs));
    gpu_init();

    vq_init(&wq);
    for (int i=0; i<ANIM_TARGETS; i++) {
        vq_init(&(fq[i]));
    }

    gic_configure(IRQ_PXI_SYNC, pxi_sync_handler);
    pxi_enable_irq();

    cpu_EnableIRQ(0);
    while(vq_count(&wq) == 0);

    /* timer_init(some_frequency, timer_handler); */

    /* start decompressing frames onto the queue */
    bax_cvt_fq(&wq, &fq);
    while(1) {
        bool qe=false;
        /* wait until all frame queues are empty */
        for (int i=0; !qe&&i<ANIM_TARGETS; i++) {
            qe |= (vq_count(&fq[i]) > 0);
        }
        if (!qe)
            break;
    }
    pxi_send_rem(PXI_CMD_FIRMBOOT);
    pxi_sync();
    while(1);
}
