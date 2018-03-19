#include <common.h>
#include <cpu.h>
#include <cache.h>
#include <mmap.h>
#include <interrupt.h>
#include <ringbuf.h>

#include "anim.h"
#include "hw/gx.h"
#include "hw/hid.h"
#include "arm/irq.h"
#include "arm/bug.h"
#include "lib/fs/fs.h"
#include "lib/lz4/lz4.h"

// Probably enough for your average anim
#define ANIM_RINGBUFFER_SIZE (128)

const GX_FrameBuffers ANIM_FramebufferLayout = {
    VRAM_FRAMEBUFFER_A, VRAM_FRAMEBUFFER_B
};

int BAX_Validate(FS_File *bax_f)
{
    BAX hdr;
    BAX_FData fdata;
    size_t bax_s, bax_data_pos;

    assert(bax_f != NULL);

    FS_FileSeek(bax_f, 0L);
    bax_s = FS_FileSize(bax_f);

    if (!bound(bax_s, sizeof(BAX), ANIM_MAX_SIZE))
        return ANIM_ERR_SIZE;

    FS_FileRead(bax_f, &hdr, sizeof(BAX));

    if (!bound(hdr.version, ANIM_MIN_VER, ANIM_MAX_VER))
        return ANIM_ERR_VERSION;

    if (!bound(hdr.frame_n, ANIM_MIN_FRAMES, ANIM_MAX_FRAMES))
        return ANIM_ERR_FRAMECOUNT;

    if (!bound(hdr.frame_r, ANIM_MIN_RATE, ANIM_MAX_RATE))
        return ANIM_ERR_FRAMERATE;

    if (!bound((s32)hdr.x_offset, ANIM_MIN_OFFSET, ANIM_MAX_OFFSET))
        return ANIM_ERR_OFFSET;

    if (!bound(hdr.x_width, ANIM_MIN_WIDTH, ANIM_MAX_WIDTH))
        return ANIM_ERR_WIDTH;

    if (!bound((hdr.x_offset + hdr.x_width) * ANIM_WIDTH_MULT, ANIM_MIN_FSIZE, ANIM_MAX_FSIZE))
        return ANIM_ERR_SIZE;

    bax_data_pos = sizeof(BAX) + (sizeof(BAX_FData) * hdr.frame_n);
    for (u32 i = 0; i < hdr.frame_n; i++) {
        FS_FileSeek(bax_f, sizeof(BAX) + (sizeof(BAX_FData) * i));
        FS_FileRead(bax_f, &fdata, sizeof(BAX_FData));
        if (!bound(BAX_FDataPos(&fdata), bax_data_pos, bax_s))
            return ANIM_ERR_INFO_OFF;

        if (!bound(BAX_FDataCompSZ(&fdata), 1, (u32)LZ4_COMPRESSBOUND(hdr.x_width * ANIM_WIDTH_MULT)))
            return ANIM_ERR_INFO_SIZE;
    }

    return ANIM_OK;
}

static RingBuffer FrameRB;
static int ANIM_PlaybackState;
static u32 ANIM_PlaybackRate, ANIM_PlaybackOff, ANIM_PlaybackSkipKey;

static void ANIM_VBlankISR(u32 irqn)
{
    static int ANIM_VBlankISRCounter = IRQ_VBLANK_FREQ;
    void *frame;
    size_t fsz;

    if (ANIM_PlaybackState < 0)
        return;

    HID_Scan();
    if (HID_Down() & ANIM_PlaybackSkipKey) {
        ANIM_PlaybackState = -1;
        return;
    }

    ANIM_VBlankISRCounter -= ANIM_PlaybackRate;
    if (ANIM_VBlankISRCounter <= 0) {
        if (RingBuffer_Fetch(&FrameRB, &frame, &fsz) == true) {
            GX_DMACopyAsync(frame, 0, (void*)(GX_NextFramebuffer() + ANIM_PlaybackOff), 0, fsz);
            ANIM_VBlankISRCounter += IRQ_VBLANK_FREQ;
            free(frame);
            GX_DMAWait();
            GX_SwapBuffers();
        } else {
            ANIM_VBlankISRCounter = 0;
        }
    }
    return;
}

static inline int BAX_DecompressFrame(char *u, size_t us, const char *c, size_t cs) {
    return LZ4_decompress_safe(c, u, cs, us);
}

static inline void *ANIM_AttemptAlloc(size_t sz, size_t a, size_t n) {
    void *ret = NULL;
    size_t n_ = n;
    CritStat cs;
    while(1) {
        cs = CPU_EnterCritical();
        ret = memalign(a, sz);
        CPU_LeaveCritical(cs);

        if (ret != NULL) break;
        if ((n_--) == 0) BUG(BUGSTR("ANIM ALLOC"), 1, BUGINT(sz, n), 2);
        CPU_WFI();
    }
    return ret;
}

void BAX_Play(FS_File *bax_f, u32 skip_hid)
{
    CritStat cs;
    int res;
    BAX hdr;
    BAX_FData *fdata;
    u32 frame, fsz, compsz;
    u32 *backbuffer, *framebuffer, *compfb;

    assert(bax_f != NULL);


    // Validate BAX header and frames
    res = BAX_Validate(bax_f);
    if (res != ANIM_OK)
        BUG(BUGSTR("ANIM VALIDATE", FS_FilePath(bax_f)), 2, BUGINT(res, FS_FileSize(bax_f)), 2);


    // Read BAX header and frames (again...)
    FS_FileSeek(bax_f, 0L);
    FS_FileRead(bax_f, &hdr, sizeof(BAX));

    fdata = malloc(sizeof(BAX_FData) * hdr.frame_n);
    assert(fdata != NULL);

    FS_FileSeek(bax_f, sizeof(BAX));
    FS_FileRead(bax_f, fdata, sizeof(BAX_FData) * hdr.frame_n);


    // Initialize state
    ANIM_PlaybackOff     = hdr.x_offset * ANIM_WIDTH_MULT;
    ANIM_PlaybackRate    = hdr.frame_r;
    ANIM_PlaybackState   = 1;
    ANIM_PlaybackSkipKey = skip_hid;
    fsz = hdr.x_width * ANIM_WIDTH_MULT;


    // Initial setup
    RingBuffer_Init(&FrameRB, ANIM_RINGBUFFER_SIZE);

    backbuffer = memalign(GX_TEXTURE_ALIGNMENT, fsz);
    compfb = malloc(LZ4_COMPRESSBOUND(fsz));
    assert(compfb != NULL);
    assert(backbuffer != NULL);
    memset(backbuffer, 0, fsz);

    // Process all configuration flags here
    // if any...


    // GPU stuff
    GX_PSCFill(VRAM_START, VRAM_SIZE, EXTENDST(hdr.clear_c), PSC_FILL32);
    GX_SetFramebuffers(&ANIM_FramebufferLayout);
    GX_SetFramebufferMode(PDC_RGB565);


    // Register VBlank interrupt handler
    cs = CPU_EnterCritical();
    IRQ_Register(IRQ_VBLANK0, ANIM_VBlankISR, 0);
    CPU_LeaveCritical(cs);

    frame = 0;
    while((frame < hdr.frame_n) && (ANIM_PlaybackState > 0)) {
        compsz = BAX_FDataCompSZ(&fdata[frame]);

        framebuffer = ANIM_AttemptAlloc(fsz, GX_TEXTURE_ALIGNMENT, MAX_ALLOC_ATTEMPTS);

        FS_FileSeek(bax_f, BAX_FDataPos(&fdata[frame]));
        FS_FileRead(bax_f, compfb, compsz);

        res = BAX_DecompressFrame((char*)framebuffer, fsz, (const char*)compfb, compsz);
        if ((u32)res != fsz)
            BUG(BUGSTR("ANIM DECOMPRESS", FS_FilePath(bax_f)), 2, BUGINT(frame, res, fsz, compsz), 4);

        // Perform delta decoding
        BAX_DeltaDecode(backbuffer, framebuffer, fsz);

        CACHE_WbDCRange(framebuffer, fsz);
        while(RingBuffer_Store(&FrameRB, framebuffer, fsz) == false) {
            if (ANIM_PlaybackState < 0) {
                free(framebuffer);
                break;
            } else {
                CPU_WFI();
            }
        }
        frame++;
    }

    // If the animation was halted, drain the buffer
    cs = CPU_EnterCritical();
    if (ANIM_PlaybackState > 0)
        ANIM_PlaybackState = 0;
    CPU_LeaveCritical(cs);

    // Wait until it's done playing
    while((RingBuffer_Count(&FrameRB) > 0) && (ANIM_PlaybackState == 0)) {
        CPU_WFI();
    }

    // Disable VBlank interrupt
    // Deallocate used memory
    cs = CPU_EnterCritical();
    IRQ_Disable(IRQ_VBLANK0, 0);
    free(fdata);
    free(compfb);
    free(backbuffer);

    while(RingBuffer_Count(&FrameRB) > 0) {
        void *b;
        RingBuffer_Fetch(&FrameRB, &b, NULL);
        free(b);
    }
    RingBuffer_Destroy(&FrameRB);
    CPU_LeaveCritical(cs);
    return;
}
