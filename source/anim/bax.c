#include <common.h>

#include <anim/bax.h>
#include <anim/frame_queue.h>

#include <gfx/gfx.h>
#include <fs/ff/ff.h>
#include <fs/fsutil.h>
#include <hw/timer.h>
#include <arm/cpu.h>

/* FIFO Queue containing frames to be displayed */
volatile DTCM frame_queue f_queue;

/* Missed frame count */
volatile DTCM uint32_t missed;

/*
 Buffer status
 = 0 : still reading from disk
 = 1 : all frames have been loaded to memory
 = -1: finish playback
*/
volatile DTCM int buffered;

void ITCM bax_timer_handler(UNUSED uint32_t xrq_id)
{
    void *frame;

    if (bax_abort()) {
        buffered = -1;
    } else if (frame_queue_count(f_queue) > 0) {
        /* Copy frames from memory to VRAM */
        /* Free the frame afterwards */
        frame = frame_queue_extract(f_queue);
        fastcpy(frame_dest(frame), frame_data(frame), frame_length(frame));
        writeback_dcache_range((uint32_t)frame_dest(frame), (uint32_t)(frame_dest(frame) + frame_length(frame)));
        free(frame_data(frame));
        frame_free(frame);
    } else if (buffered) {
        /* No frames are available and all frames have already been loaded */
        /* Finish playback, stop timer */
        timer_stop(BAX_TIMER);
        buffered = -1;
    } else {
        /* There's frames in disk but not in memory */
        /* Increase missed frames counter */
        missed++;
    }
    return;
}

void bax_loop(bax_header *anim_hdr, FIL *anim_fil)
{
    void *f_buf, *top_fb, *bot_fb;
    uint32_t ss;
    size_t f_sz;

    /* Initialize state */
    top_fb = get_framebuffer(GFX_TOP);
    bot_fb = get_framebuffer(GFX_BOTTOM);
    f_queue = frame_queue_init();
    buffered = 0;
    missed = 0;

    f_lseek(anim_fil, sizeof(bax_header));
    timer_init(BAX_TIMER, SECOND / anim_hdr->rate, TIMER_PERIODIC, bax_timer_handler);

    /*
     For each screen:
        - allocate FRAME_SIZE bytes
        - if allocation fails, skip
        - otherwise, read FRAME_SIZE from disk
        - if reading fails, break out
        - otherwise, add to frame queue
        - repeat

     Allocator and frame queue ops are not thread-safe, as
     such they must be handled inside critical sections.
    */
    while((anim_hdr->topcnt || anim_hdr->botcnt) && (buffered == 0)) {
        if (anim_hdr->topcnt > 0) {
            ENTER_CRITICAL(ss);
            f_buf = malloc(TOP_FRAME_SIZE);
            LEAVE_CRITICAL(ss);
            if (!f_buf) {
                continue;
            }

            f_read(anim_fil, f_buf, TOP_FRAME_SIZE, &f_sz);

            if (!f_sz) {
                break;
            }

            ENTER_CRITICAL(ss);
            frame_queue_add(f_queue, f_sz, top_fb, f_buf);
            LEAVE_CRITICAL(ss);

            anim_hdr->topcnt--;
        }

        if (anim_hdr->botcnt > 0) {
            ENTER_CRITICAL(ss);
            f_buf = malloc(BOTTOM_FRAME_SIZE);
            LEAVE_CRITICAL(ss);
            if (!f_buf) {
                continue;
            }

            f_read(anim_fil, f_buf, BOTTOM_FRAME_SIZE, &f_sz);

            if (!f_sz) {
                break;
            }

            ENTER_CRITICAL(ss);
            frame_queue_add(f_queue, f_sz, bot_fb, f_buf);
            LEAVE_CRITICAL(ss);

            anim_hdr->botcnt--;
        }
    }

    ENTER_CRITICAL(ss);
    if (buffered == 0) {
        buffered = 1;
    }
    LEAVE_CRITICAL(ss);

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
