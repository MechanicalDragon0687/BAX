#include <common.h>
#include <cache.h>
#include <cpu.h>
#include <mmap.h>
#include <interrupt.h>
#include <rb.h>

#include "anim.h"
#include "hw/gx.h"
#include "hw/hid.h"
#include "hw/timer.h"
#include "arm/irq.h"
#include "arm/bugcheck.h"
#include "lib/lz4/lz4.h"

const gx_framebuffers_t anim_framebuffer_layout =
{
    { // top left screen
        VRAM_START,
        VRAM_START + VRAM_TOP_DIM*4 + VRAM_BOTTOM_DIM*2
    },
    { // top right screen
        VRAM_START + VRAM_TOP_DIM*2 + VRAM_BOTTOM_DIM*2,
        VRAM_START + VRAM_TOP_DIM*6 + VRAM_BOTTOM_DIM*4
    },
    { // bottom screen
        VRAM_START + VRAM_TOP_DIM*2,
        VRAM_START + VRAM_TOP_DIM*6 + VRAM_BOTTOM_DIM*2
    }
    // <top left, bottom, top right>
};

int anim_validate(const anim_t *hdr, size_t hdr_sz)
{
    u32 frame = 0, info_end;
    if (hdr == NULL || hdr_sz <= sizeof(anim_t)) return ANIM_ERR_MEM;
    if (memcmp(hdr->magic, ANIM_MAGIC, sizeof(hdr->magic)) != 0) return ANIM_ERR_MAGIC;
    if (hdr->version < ANIM_MIN_VER || hdr->version > ANIM_MAX_VER) return ANIM_ERR_VERSION;
    if (hdr->frame_n < ANIM_MIN_FRAMES || hdr->frame_n > ANIM_MAX_FRAMES) return ANIM_ERR_FRAMECOUNT;
    if (hdr->frame_r < ANIM_MIN_RATE || hdr->frame_r > ANIM_MAX_RATE) return ANIM_ERR_FRAMERATE;
    if (hdr->x_offset < ANIM_MIN_OFFSET || hdr->x_offset > ANIM_MAX_OFFSET) return ANIM_ERR_OFFSET;
    if (hdr->x_length < ANIM_MIN_LENGTH || hdr->x_length > ANIM_MAX_LENGTH) return ANIM_ERR_WIDTH;
    if ((hdr->x_offset + hdr->x_length)*ANIM_WIDTH_MULT < ANIM_MIN_FSIZE ||
        (hdr->x_offset + hdr->x_length)*ANIM_WIDTH_MULT > ANIM_MAX_FSIZE) return ANIM_ERR_SIZE;

    for (frame = 0, info_end = sizeof(anim_t) + (sizeof(anim_finfo_t) * hdr->frame_n); frame < hdr->frame_n; frame++)
    {
        if (hdr->frame_info[frame].offset < info_end || hdr->frame_info[frame].offset > hdr_sz) return ANIM_ERR_INFO_OFF;
        if (hdr->frame_info[frame].compsz > LZ4_COMPRESSBOUND(ANIM_MAX_FSIZE)) return ANIM_ERR_INFO_SIZE;
    }
    return ANIM_OK;
}

static rb_t ringbuf;
static bool anim_playback;
static u32 frame_rate, frame_size, frame_offset;
static void anim_vblank_isr(u32 irqn)
{
    u32 *frame;
    static u32 _anim_vblank_isr_ctr = 0;

    hid_scan();
    _anim_vblank_isr_ctr++;
    if (hid_down() & HID_X)
    {
        anim_playback = false;
        while(rb_count(&ringbuf) > 0)
            free(rb_fetch(&ringbuf, NULL));
    }
    else if ((_anim_vblank_isr_ctr*frame_rate) >= ANIM_MAX_RATE)
    {
        frame = rb_fetch(&ringbuf, NULL);
        if (frame != NULL)
        {
            gx_dma_copy(frame, 0, (u32*)(gx_next_framebuffer(GFX_TOPL) + frame_offset), 0, frame_size);
            free(frame);
            gx_swap_buffers();
            _anim_vblank_isr_ctr = 0;
        }
    }
    return;
}

int anim_play(const anim_t *hdr)
{
    bool stored;
    u32 crit_status, alloc_attempts, frame;
    u32 *backbuffer, *framebuffer;

    // Initialize state
    frame_offset = hdr->x_offset * ANIM_WIDTH_MULT;
    frame_size = hdr->x_length * ANIM_WIDTH_MULT;
    frame_rate = hdr->frame_r;
    alloc_attempts = 0;
    anim_playback = true;
    frame = 0;

    // Initial setup
    rb_init(&ringbuf);
    backbuffer = memalign(GX_TEXTURE_ALIGNMENT, frame_size);
    if (backbuffer == NULL)
        return ANIM_ERR_MEM;

    memset(backbuffer, 0, frame_size);

    /* Process all configuration flags here */
    gx_psc_fill(VRAM_START, VRAM_SIZE, EXTENDST(hdr->clear_c), PSC_FILL32);
    gx_set_framebuffers(&anim_framebuffer_layout);
    gx_set_framebuffer_mode(PDC_RGB565);

    // Register VBlank and Timer interrupt handlers
    crit_status = _enter_critical();
    irq_register(IRQ_VBLANK0, anim_vblank_isr, 0);
    _leave_critical(crit_status);

    while(frame < hdr->frame_n && anim_playback)
    {
        // Allocate frame in memory
        // must be GPU aligned
        crit_status = _enter_critical();
        framebuffer = memalign(GX_TEXTURE_ALIGNMENT, frame_size);
        _leave_critical(crit_status);

        // hopefully out of RAM and not heap corruption?
        if (framebuffer == NULL)
        {
            if (++alloc_attempts > MAX_ALLOC_ATTEMPTS)
                bugcheck("ANIM_MEMALIGN", (u32[]){frame}, 1);

            // in case it actually is out of memory
            // give the ring buffer some time to be drained
            _wfi();
            continue;
        }
        alloc_attempts = 0; // good, reset the fail counter

        if ((u32)LZ4_decompress_safe(
            anim_frame_data(hdr, frame),
            (char*)framebuffer,
            hdr->frame_info[frame].compsz,
            frame_size) != frame_size)
        {
            // bad decompression
            bugcheck("ANIM_DECOMPRESS", (u32[]){frame}, 1);
        }

        // Perform delta decoding
        // TODO: unroll this if I ever find a slow anim
        for (u32 q = 0; q < (frame_size / sizeof(u32)); q++)
        {
            u32 delta = backbuffer[q] + framebuffer[q];
            backbuffer[q] = delta;
            framebuffer[q] = delta;
        }

        // GPU DMA is fun
        _writeback_DC_range(framebuffer, frame_size);

        // Interrupts are fun
        do
        {
            crit_status = _enter_critical();
            stored = rb_store(&ringbuf, framebuffer, frame_size);
            _leave_critical(crit_status);

            // Not enough space
            if (stored == false) _wfi();
        } while(stored == false);

        frame++;
    }

    // If the animation was exited, drain the buffer
    if (anim_playback == false)
    {
        crit_status = _enter_critical();
        while(rb_count(&ringbuf) > 0)
            free(rb_fetch(&ringbuf, NULL));
        _leave_critical(crit_status);
    }

    // Wait until it's done playing
    while(rb_count(&ringbuf) > 0) _wfi();

    // Deallocate used memory
    // Disable VBlank and Timer interrupts
    crit_status = _enter_critical();
    free(backbuffer);
    timer_stop();
    irq_deregister(IRQ_VBLANK0, 0);
    _leave_critical(crit_status);
    return ANIM_OK;
}
