#include <common.h>
#include <anim.h>
#include <cpu.h>
#include <lz4.h>
#include <vq.h>

/* TODO use DMA ffs */
#define MAX_MALLOC_TRIES (0x100000)

/*
 Fetches a single frame from a frame queue
 and streams it to its target
 Not interrupt-safe
*/
void bax_stream_fq(u32 tgt, vq *fq)
{
    u32 tgt_len;
    void *data;

    if (vq_count(fq) == 0)
        return;

    tgt_len = anim_target_ulen[tgt];
    data = vq_fetch(fq);
    vq_remove(fq);
    switch(tgt) {
    case TARGET_TOPLEFT:
        memcpy(get_framebuffer(GFX_MAIN), data, tgt_len);
        break;
    case TARGET_TOPRIGHT:
        memcpy(get_framebuffer(GFX_MAINR), data, tgt_len);
        break;
    case TARGET_BOTTOM:
        memcpy(get_framebuffer(GFX_SUB), data, tgt_len);
        break;
    default:
        break;
    }
    return;
}

/*
 Fetches frames from a work queue, decompresses
 them, applies delta encoding and adds them
 to their corresponding frame queue
 Interrupt-safe
 Returns false on allocation failure
*/
bool bax_cvt_fq(vq *wq, vq (*fq)[ANIM_TARGETS])
{
    anim_frame *frame;
    u32 bmc=0, f=0, i, ss, stat, tgt_ulen;
    bool ret=true;
    void *backbuf[ANIM_TARGETS], *decompbuf[ANIM_TARGETS], *strmbuf;

    /* Allocate initial buffers */
    ENTER_CRITICAL(ss);
    for (i=0; i<ANIM_TARGETS; i++) {
        tgt_ulen = anim_target_ulen[i];
        backbuf[i] = malloc(tgt_ulen);
        decompbuf[i] = malloc(tgt_ulen);
        if (backbuf[i] && decompbuf[i]) {
            memset(backbuf[i], 0, tgt_ulen);
            memset(decompbuf[i], 0, tgt_ulen);
        } else {
            ret=false;
            break;
        }
    }
    LEAVE_CRITICAL(ss);

    while(ret && vq_count(wq)>0) {
        ENTER_CRITICAL(ss);
        frame = (anim_frame*)vq_fetch(wq);
        tgt_ulen = anim_target_ulen[frame->tgt];
        strmbuf = malloc(tgt_ulen);
        LEAVE_CRITICAL(ss);

        if (!strmbuf) {
            if (bmc > MAX_MALLOC_TRIES) {
                ret = false;
            }
            bmc++;
            continue;
        }

        stat = LZ4_decompress_safe(&(frame->strm), decompbuf[frame->tgt], frame->clen, tgt_ulen);
        if (stat == tgt_ulen) {
            for (u32 i = 0; i < tgt_ulen/4; i++) {
                ((u32*)backbuf)[i] += ((u32*)decompbuf)[i];
            }
            memcpy(strmbuf, backbuf, tgt_ulen);
            ENTER_CRITICAL(ss);
            vq_remove(wq);
            vq_add(fq[frame->tgt], strmbuf);
            LEAVE_CRITICAL(ss);
        } else {
            ENTER_CRITICAL(ss);
            free(strmbuf);
            LEAVE_CRITICAL(ss);
        }

        bmc=0;
        f++;
    }

    ENTER_CRITICAL(ss);
    for (i=0; i<ANIM_TARGETS; i++) {
        if (backbuf[i]) free(backbuf[i]);
        if (decompbuf[i]) free(backbuf[i]);
    }
    LEAVE_CRITICAL(ss);
    return ret;
}

/*
 Fetches frames from an animation
 and adds them to a work queue
 Not interrupt-safe
*/
u32 bax_cvt_wq(void *anim, u32 size, vq *wq)
{
    anim_header *hdr;
    anim_frame *frme;
    u32 ret=0;

    hdr = (anim_header*)anim;
    if (anim_header_valid(hdr, size)) {
        frme = (anim_frame*)(anim + hdr->foff);
        while(anim_frame_valid(frme)) {
            if (!vq_add(wq, frme)) {
                break;
            }
            frme = (anim_frame*)(frme + sizeof(anim_frame) + frme->clen);
            ret++;
        }
    }
    return ret;
}
