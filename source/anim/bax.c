#include <common.h>

#include <anim/bax.h>
#include <anim/frame_queue.h>

#include <gfx/gfx.h>
#include <fs/ff/ff.h>
#include <fs/fsutil.h>
#include <hw/timer.h>
#include <arm/arm.h>

volatile DTCM frame_queue f_queue;
volatile DTCM bool buffered;
volatile DTCM uint32_t missed;

void ITCM bax_timer_handler(UNUSED uint32_t xrq_id)
{
    void *frame;
    if (frame_queue_count(f_queue)) {
        frame = frame_queue_extract(f_queue);
        fastcpy(frame_dest(frame), frame_data(frame), frame_length(frame));
        flush_dcache_range(frame_dest(frame), frame_dest(frame) + frame_length(frame));

        /*
         I'm not too happy about freeing memory from a critical section
         I should look into how fast the operation is, or write my own memory manager
        */

        free(frame_data(frame));
        free(frame);
    } else if (buffered) {
        timer_stop(BAX_TIMER);
        buffered = false;
    } else {
        missed++;
    }
    return;
}

void bax_loop(bax_header *anim_hdr, FIL *anim_fil)
{
    void *f_buf, *top_fb, *bot_fb;
    size_t f_sz;

    top_fb = get_framebuffer(GFX_TOP);
    bot_fb = get_framebuffer(GFX_BOTTOM);

    f_queue = frame_queue_init();
    buffered = false;
    missed = 0;

    f_lseek(anim_fil, sizeof(bax_header));
    timer_init(BAX_TIMER, SECOND / anim_hdr->rate, -1, bax_timer_handler);

    while(anim_hdr->topcnt || anim_hdr->botcnt) {
        if (anim_hdr->topcnt > 0) {
            f_buf = malloc(TOP_FRAME_SIZE);
            if (!f_buf) continue;

            f_read(anim_fil, f_buf, TOP_FRAME_SIZE, &f_sz);

            /* todo: only disable irqs when the queue is too smol */
            irq_disable();
            frame_queue_add(f_queue, f_sz, top_fb, f_buf);
            irq_enable();

            anim_hdr->topcnt--;
        }

        if (anim_hdr->botcnt > 0) {
            f_buf = malloc(BOTTOM_FRAME_SIZE);
            if (!f_buf) continue;

            f_read(anim_fil, f_buf, BOTTOM_FRAME_SIZE, &f_sz);

            irq_disable();
            frame_queue_add(f_queue, f_sz, bot_fb, f_buf);
            irq_enable();

            anim_hdr->botcnt--;
        }
    }

    buffered = true;
    return;
}

volatile bool *bax_start(void)
{
    FIL bax_fp;
    FRESULT res;
    size_t bax_sz, br;
    bax_header anim_hdr;
    char *bax_f;

    buffered = false;
    bax_f = get_random_file(BASE_PATH, "*.bax", MAX_ANIMATIONS);
    if (f_open(&bax_fp, bax_f, FA_READ) != FR_OK)
        return &buffered;

    bax_sz = f_size(&bax_fp);
    if (bax_sz <= sizeof(bax_header)) {
        goto _bax_end;
    }

    res = f_read(&bax_fp, &anim_hdr, sizeof(bax_header), &br);
    if (res != FR_OK || br != sizeof(bax_header) ||
        !bax_magic_valid(&anim_hdr)) {
        goto _bax_end;
    }

    bax_loop(&anim_hdr, &bax_fp);

_bax_end:
    f_close(&bax_fp);
    return &buffered;
}
