#include <common.h>
#include <cpu.h>
#include <cache.h>
#include <mmap.h>
#include <lock.h>
#include <interrupt.h>
#include <ringbuf.h>

#include "anim.h"
#include "hw/gx.h"
#include "hw/hid.h"
#include "arm/irq.h"
#include "arm/bug.h"
#include "lib/fs/fs.h"
#include "lib/lz4/lz4.h"

const GX_FrameBuffers ANIM_FramebufferLayout = {
    VRAM_FRAMEBUFFER_A, VRAM_FRAMEBUFFER_B
};

int BAX_Validate(FS_File *bax_f)
{
    BAX hdr;
    BAX_FData *fdata;
    size_t bax_s, bax_data_pos;

    assert(bax_f != NULL);

    FS_FileSetPos(bax_f, 0L);
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

    fdata = LockMalloc(sizeof(BAX_FData) * hdr.frame_n);
    assert(fdata != NULL);

    FS_FileSetPos(bax_f, sizeof(BAX));
    FS_FileRead(bax_f, fdata, sizeof(BAX_FData) * hdr.frame_n);
    for (u32 i = 0; i < hdr.frame_n; i++) {
        if (!bound(BAX_FDataPos(&fdata[i]), bax_data_pos, bax_s))
            return ANIM_ERR_INFO_OFF;

        if (!bound(BAX_FDataCompSZ(&fdata[i]), 1, (u32)LZ4_COMPRESSBOUND(hdr.x_width * ANIM_WIDTH_MULT)))
            return ANIM_ERR_INFO_SIZE;
    }
    LockFree(fdata);

    return ANIM_OK;
}

static RingBuffer FrameRB;
static int ANIM_PlaybackState;
static u32 ANIM_PlaybackRate, ANIM_PlaybackOff;

static inline void ANIM_DrainFreeRB(RingBuffer *rb) {
    CritStat cs = CPU_EnterCritical();
    void *b;
    while(RingBuffer_Count(rb) > 0) {
        RingBuffer_Fetch(rb, &b, NULL);
        free(b);
    }
    CPU_LeaveCritical(cs);
}

static void ANIM_VBlankISR(u32 irqn)
{
    static u32 ANIM_VBlankISRCounter = 0;
    static void *f = NULL;
    size_t fsz;

    HID_Scan();
    ANIM_VBlankISRCounter++;

    if (HID_Down() & HID_X) {
        ANIM_PlaybackState = -1;
        ANIM_DrainFreeRB(&FrameRB);
        if (f != NULL) free(f);
    }

    if (ANIM_VBlankISRCounter == 1) {
        if (RingBuffer_Fetch(&FrameRB, &f, &fsz) == true) {
            GX_DMAWait();
            GX_DMACopyAsync(f, 0, (void*)(GX_NextFramebuffer() + ANIM_PlaybackOff), 0, fsz);
        } else {
            ANIM_VBlankISRCounter = 0;
            f = NULL;
        }
    }

    if ((ANIM_VBlankISRCounter * ANIM_PlaybackRate) >= ANIM_MAX_RATE) {
        free(f);
        f = NULL;

        GX_DMAWait();
        GX_SwapBuffers();
        ANIM_VBlankISRCounter = 0;
    }
    return;
}

static inline int BAX_DecompressFrame(char *u, size_t us, const char *c, size_t cs) {
    return LZ4_decompress_safe(c, u, cs, us);
}

static inline void BAX_DeltaDecode(void *bb, void *fb, size_t l) {
    u32 *bb_, *fb_, delta;

    bb_ = (u32*)bb;
    fb_ = (u32*)fb;
    l /= sizeof(u32);
    while(l--) {
        delta = bb_[l] + fb_[l];
        fb_[l] = delta;
        bb_[l] = delta;
    }
}

static inline void *ANIM_AttemptAlloc(size_t sz, size_t a, size_t n) {
    void *ret;
    size_t n_ = n;
    while(1) {
        ret = LockMemalign(a, sz);
        if (ret != NULL)
            break;

        if (n_--) BUG(BUGSTR("ANIM_ALLOC"), 1, BUGINT(sz, n), 2);
        CPU_WFI();
    }
    return ret;
}

void BAX_Play(FS_File *bax_f)
{
    CritStat cs;
    int res;
    BAX hdr;
    BAX_FData *fdata;
    u32 frame, memfail, fsz, compsz;
    u32 *backbuffer, *framebuffer, *compfb;

    assert(bax_f != NULL);


    // Validate BAX header and frames
    res = BAX_Validate(bax_f);
    if (res != ANIM_OK)
        BUG(BUGSTR("ANIM_VALIDATE", FS_FilePath(bax_f)), 2, BUGINT(res, FS_FileSize(bax_f)), 2);


    // Read BAX header and frames (again...)
    FS_FileSetPos(bax_f, 0L);
    FS_FileRead(bax_f, &hdr, sizeof(BAX));

    fdata = LockMalloc(sizeof(BAX_FData) * hdr.frame_n);
    assert(fdata != NULL);

    FS_FileSetPos(bax_f, sizeof(BAX));
    FS_FileRead(bax_f, fdata, sizeof(BAX_FData) * hdr.frame_n);


    // Initialize state
    ANIM_PlaybackOff   = hdr.x_offset * ANIM_WIDTH_MULT;
    ANIM_PlaybackRate  = hdr.frame_r;
    ANIM_PlaybackState = 1;
    fsz = hdr.x_width * ANIM_WIDTH_MULT;
    frame = 0;


    // Initial setup
    RingBuffer_Init(&FrameRB, 64);

    backbuffer = LockMemalign(GX_TEXTURE_ALIGNMENT, fsz);
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

    while((frame < hdr.frame_n) && (ANIM_PlaybackState >= 0)) {
        compsz = BAX_FDataCompSZ(&fdata[frame]);
        compfb = ANIM_AttemptAlloc(compsz, 8, MAX_ALLOC_ATTEMPTS);
        framebuffer = ANIM_AttemptAlloc(fsz, GX_TEXTURE_ALIGNMENT, MAX_ALLOC_ATTEMPTS);

        FS_FileSetPos(bax_f, BAX_FDataPos(&fdata[frame]));
        FS_FileRead(bax_f, compfb, compsz);

        res = BAX_DecompressFrame((char*)framebuffer, fsz, (const char*)compfb, compsz);
        if (res <= 0)
            BUG(BUGSTR("ANIM_DECOMPRESS"), 1, BUGINT(frame, res, fsz, compsz), 4);

        // Perform delta decoding
        // TODO: unroll this if I ever find a slow anim
        BAX_DeltaDecode(backbuffer, framebuffer, fsz);

        CACHE_WbDCRange(framebuffer, fsz);
        while(RingBuffer_Store(&FrameRB, framebuffer, fsz) == false) CPU_WFI();

        LockFree(compfb);
        frame++;
    }

    // If the animation was exited, drain the buffer
    cs = CPU_EnterCritical();
    if (ANIM_PlaybackState < 0) {
        ANIM_DrainFreeRB(&FrameRB);
    } else {
        ANIM_PlaybackState = 0;
    }
    CPU_LeaveCritical(cs);

    // Wait until it's done playing
    while(RingBuffer_Count(&FrameRB) > 0) CPU_WFI();

    // Deallocate used memory
    // Disable VBlank interrupt
    cs = CPU_EnterCritical();
    free(fdata);
    free(backbuffer);
    RingBuffer_Destroy(&FrameRB);
    IRQ_Disable(IRQ_VBLANK0, 0);
    CPU_LeaveCritical(cs);
    return;
}
