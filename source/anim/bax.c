#include <common.h>

#include <anim/bax.h>
#include <anim/frame_queue.h>

#include <gfx/gfx.h>
#include <fs/ff/ff.h>
#include <fs/fsutil.h>
#include <hw/timer.h>
#include <arm/cpu.h>

#include <hw/hid.h>

volatile DTCM frame_queue f_queue;
volatile DTCM uint32_t missed;
volatile DTCM int buffered;
/*
 buffered == 0 : not all frames are on queue
 buffered == 1 : all frames are on queue
 buffered == -1: abort playback
*/

void ITCM bax_timer_handler(UNUSED uint32_t xrq_id)
{
    void *frame;

    if (abort_anim()) {
        buffered = -1;
    } else if (frame_queue_count(f_queue)) {
        frame = frame_queue_extract(f_queue);

        /* alternative: make the GPU point to frame_data(frame) */
        fastcpy(frame_dest(frame), frame_data(frame), frame_length(frame));
        writeback_dcache_range((uint32_t)frame_dest(frame), (uint32_t)(frame_dest(frame) + frame_length(frame)));
        free(frame_data(frame));
        free(frame);
    } else if (buffered) {
        timer_stop(BAX_TIMER);
        buffered = 0;
    } else {
        missed++;
    }
    return;
}

void bax_loop(bax_header *anim_hdr, FIL *anim_fil)
{
    void *f_buf, *top_fb, *bot_fb;
    uint32_t ss;
    size_t f_sz;

    top_fb = get_framebuffer(GFX_TOP);
    bot_fb = get_framebuffer(GFX_BOTTOM);

    f_queue = frame_queue_init();
    buffered = 0;
    missed = 0;

    f_lseek(anim_fil, sizeof(bax_header));
    timer_init(BAX_TIMER, SECOND / anim_hdr->rate, TIMER_PERIODIC, bax_timer_handler);

    while((anim_hdr->topcnt || anim_hdr->botcnt) && (buffered == 0)) {
        if (anim_hdr->topcnt > 0) {
            f_buf = malloc(TOP_FRAME_SIZE);
            if (!f_buf) {
                continue;
            }

            f_read(anim_fil, f_buf, TOP_FRAME_SIZE, &f_sz);

            ss = irq_kill();
            frame_queue_add(f_queue, f_sz, top_fb, f_buf);
            irq_restore(ss);

            anim_hdr->topcnt--;
        }

        if (anim_hdr->botcnt > 0) {
            f_buf = malloc(BOTTOM_FRAME_SIZE);
            if (!f_buf) {
                continue;
            }

            f_read(anim_fil, f_buf, BOTTOM_FRAME_SIZE, &f_sz);

            ss = irq_kill();
            frame_queue_add(f_queue, f_sz, bot_fb, f_buf);
            irq_restore(ss);

            anim_hdr->botcnt--;
        }
    }

    ss = irq_kill();
    if (buffered == 0) {
        buffered = 1;
    }
    irq_restore(ss);

    while(buffered == 1);
    frame_queue_kill(f_queue);
    return;
}

void bax_start(void)
{
    FIL bax_fp;
    size_t br;
    bax_header anim_hdr;

    if (f_open(&bax_fp, get_random_file(BASE_PATH, "*.bax", MAX_ANIMATIONS), FA_READ) == FR_OK) {
        f_read(&bax_fp, &anim_hdr, sizeof(bax_header), &br);
        if (br == sizeof(bax_header) && bax_hdr_valid(&anim_hdr)) {
            bax_loop(&anim_hdr, &bax_fp);
        }
        f_close(&bax_fp);
    }

    return;
}
