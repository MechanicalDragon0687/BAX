#include <common.h>
#include <cache.h>
#include <cpu.h>
#include <irq.h>
#include <vram.h>

#include "hw/timer.h"
#include "hw/int.h"
#include "hw/hid.h"
#include "hw/gx.h"
#include "anim.h"
#include "lib/lz4/lz4.h"
#include "lib/frb/frb.h"

static frb_t frb;
static bool swap_on_vblank;

const gx_framebuffers_t framebuffer_layouts[2] =
{
    // Non-stereoscopic layout
    {{
        { // top left screen
            VRAM_START,
            VRAM_START + VRAM_TOP_SIZE*2 + VRAM_BOT_SIZE
        },
        { // top right screen
            VRAM_START + VRAM_TOP_SIZE + VRAM_BOT_SIZE,
            VRAM_START + VRAM_TOP_SIZE*3 + VRAM_BOT_SIZE*2
        },
        { // bottom screen
            VRAM_START + VRAM_TOP_SIZE,
            VRAM_START + VRAM_TOP_SIZE*3 + VRAM_BOT_SIZE
        }
        // <top left, bottom, top right>
    }},

    // Stereoscopic layout
    {{
        { // top left screen
            VRAM_START,
            VRAM_START + VRAM_TOP_SIZE*2 + VRAM_BOT_SIZE
        },
        { // top right screen
            VRAM_START + VRAM_TOP_SIZE,
            VRAM_START + VRAM_TOP_SIZE*3 + VRAM_BOT_SIZE
        },
        { // bottom screen
            VRAM_START + VRAM_TOP_SIZE*2,
            VRAM_START + VRAM_TOP_SIZE*3 + VRAM_BOT_SIZE*2
        }
        // <top left, top right, bottom>
    }}
};

const char *anim_errstr[ANIM_ERR_UNKNOWN] =
{
    [ANIM_OK]                = "No error",
    [ANIM_ERR_MEM]           = "Memory error",
    [ANIM_ERR_MAGIC]         = "Not a BAX file",
    [ANIM_ERR_VERSION]       = "Unsupported version",
    [ANIM_ERR_FRAMECOUNT]    = "Invalid frame count",
    [ANIM_ERR_FRAMERATE]     = "Invalid frame rate",
    [ANIM_ERR_OFFSET]        = "Invalid frame offset",
    [ANIM_ERR_WIDTH]         = "Invalid frame width",
    [ANIM_ERR_SIZE]          = "Invalid frame size",
    [ANIM_ERR_INFO_OFF]      = "Invalid frame info (offset)",
    [ANIM_ERR_INFO_SIZE]     = "Invalid frame info (compressed size)",
    [ANIM_ERR_DECOMPRESSION] = "Error during decompression",
};

const char *anim_get_error(int err)
{
    if (err >= 0 && err < ANIM_ERR_UNKNOWN)
        return anim_errstr[err];
    return NULL;
}

/*
 TODO:
 - Do all timer logic in the vblank interrupt instead (for some reason it fires @ 30fps?)
 - Instead of doing TextureCopy, change the fb addr reg (should work from vblank)
*/
static void anim_timer_isr(u32 irqn)
{
    u32 *frame = frb_fetch(&frb);
    if (frame != NULL)
    {
        gx_dma_copy(frame, 0, (u32*)(gx_next_framebuffer(GFX_TOPL) + frb_offset(&frb)), 0, frb_size(&frb));
        free(frame);
        swap_on_vblank = true;
    }
    return;
}

static void anim_vblank_isr(u32 irqn)
{
    hid_scan();
    if ((hid_held() & HID_DOWN) == HID_DOWN)
    {
        // drain the buffer, tell the main code to stahp
    }
    else if (swap_on_vblank)
    {
        gx_swap_buffers();
        swap_on_vblank = false;
    }
    return;
}

int anim_validate(const anim_t *hdr, size_t hdr_sz)
{
    u32 frame, info_end;
    if (hdr == NULL || hdr_sz <= sizeof(anim_t))
        return ANIM_ERR_MEM;

    if (memcmp(hdr->magic, ANIM_MAGIC, sizeof(hdr->magic)) != 0)
        return ANIM_ERR_MAGIC;

    if (hdr->version < ANIM_MIN_VER || hdr->version > ANIM_MAX_VER)
        return ANIM_ERR_VERSION;

    if (hdr->frame_n < ANIM_MIN_FRAMES || hdr->frame_n > ANIM_MAX_FRAMES)
        return ANIM_ERR_FRAMECOUNT;

    if (hdr->frame_r < ANIM_MIN_RATE || hdr->frame_r > ANIM_MAX_RATE)
        return ANIM_ERR_FRAMERATE;

    if (hdr->x_offset < ANIM_MIN_OFFSET || hdr->x_offset > ANIM_MAX_OFFSET)
        return ANIM_ERR_OFFSET;

    if (hdr->x_length < ANIM_MIN_LENGTH || hdr->x_length > ANIM_MAX_LENGTH)
        return ANIM_ERR_WIDTH;

    if ((hdr->x_offset + hdr->x_length)*ANIM_OFFSET_MULT < ANIM_MIN_FSIZE ||
        (hdr->x_offset + hdr->x_length)*ANIM_OFFSET_MULT > ANIM_MAX_FSIZE)
        return ANIM_ERR_SIZE;

    frame = 0;
    info_end = sizeof(anim_t) + (sizeof(anim_frame_info_t) * hdr->frame_n);
    while(frame < hdr->frame_n)
    {
        if (hdr->frame_info[frame].offset < info_end || hdr->frame_info[frame].offset > hdr_sz)
            return ANIM_ERR_INFO_OFF;

        if (hdr->frame_info[frame].compsz > LZ4_COMPRESSBOUND(ANIM_MAX_FSIZE))
            return ANIM_ERR_INFO_SIZE;

        frame++;
    }
    return ANIM_OK;
}

int anim_play(const anim_t *hdr)
{
    int ret = ANIM_OK;
    u32 crit_status, alloc_attempts, frame;
    u32 *backbuffer, *framebuffer;

    // Initial setup
    frb_init(&frb, hdr->x_offset * ANIM_OFFSET_MULT, hdr->x_length * ANIM_OFFSET_MULT);
    backbuffer = memalign(GX_TEXTURE_ALIGNMENT, frb_size(&frb));
    if (backbuffer == NULL)
        return ANIM_ERR_MEM;

    memset(backbuffer, 0, frb_size(&frb));

    /* Process all configuration flags here */
    gx_psc_fill(VRAM_START, VRAM_SIZE, EXTENDST(hdr->clear_c), PSC_FILL32);
    if (hdr->flags & ANIM_FLAG_STEREOSCOPIC)
    {
        gx_set_framebuffers(&framebuffer_layouts[1]);
        gx_set_stereoscopy(true);
    }
    else
    {
        gx_set_framebuffers(&framebuffer_layouts[0]);
        gx_set_stereoscopy(false);
    }

    alloc_attempts = 0;
    frame = 0;
    swap_on_vblank = false;

    // Register VBlank and Timer interrupt handlers
    crit_status = _enter_critical();
    irq_register(IRQ_VBLANK0, anim_vblank_isr);
    irq_register(IRQ_TIMER, anim_timer_isr);
    _leave_critical(crit_status);

    // Start timer, triggers an interrupt (hdr->frame_r) times per second and autoreloads
    timer_start(timer_ms_to_ticks(1000.0f/(float)hdr->frame_r), true, true);

    while(frame < hdr->frame_n)
    {
        // Allocate frame in memory
        // must be GPU aligned
        crit_status = _enter_critical();
        framebuffer = memalign(GX_TEXTURE_ALIGNMENT, frb_size(&frb));
        _leave_critical(crit_status);

        // hopefully out of RAM and not heap corruption?
        if (framebuffer == NULL)
        {
            alloc_attempts++;
            if (alloc_attempts > MAX_ALLOC_ATTEMPTS)
            {
                // last ditch measure
                // TODO: bugcheck
                crit_status = _enter_critical();
            }

            // in case it actually is out of memory
            // give the ring buffer some time to be drained
            _wfe();
            continue;
        }
        alloc_attempts = 0; // good, reset the fail counter

        if ((u32)LZ4_decompress_safe(
            (const char*)anim_get_frame_data(hdr, frame),
            (char*)framebuffer,
            hdr->frame_info[frame].compsz,
            frb_size(&frb)) != frb_size(&frb))
        {
            // bad decompression
            // TODO: bugcheck
            _enter_critical();
            while(1) _wfi();
        }

        // Perform delta decoding
        // TODO: possibly unroll this?
        for (u32 q = 0; q < (frb_size(&frb) / sizeof(u32)); q++)
        {
            u32 delta = backbuffer[q] + framebuffer[q];
            backbuffer[q] = delta;
            framebuffer[q] = delta;
        }

        // GPU DMA is fun
        _writeback_DC_range(framebuffer, frb_size(&frb));

        // Interrupts are fun
        crit_status = _enter_critical();
        frb_store(&frb, framebuffer);
        _leave_critical(crit_status);

        frame++;
    }

    // Wait until it's done playing
    while(frb_count(&frb) > 0) _wfe();

    // Deallocate used memory
    // Disable VBlank and Timer interrupts
    crit_status = _enter_critical();
    free(backbuffer);
    timer_stop();
    irq_deregister(IRQ_TIMER);
    irq_deregister(IRQ_VBLANK0);
    _leave_critical(crit_status);

    return 0;
}
