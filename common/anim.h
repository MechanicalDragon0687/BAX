#pragma once
#include <common.h>
#include <gfx.h>

#define ANIM_HEADER_MAGIC ('B' | 'A'<<8 | 'X'<<16 | 'X'<<24)
#define ANIM_FRAME_MAGIC  ('F' | 'R'<<8 | 'M'<<16 | 'E'<<24)

#define ANIM_MAXRATE  (60)

enum {
    TARGET_TOPLEFT  = 0,
    TARGET_TOPRIGHT = 1,
    TARGET_BOTTOM   = 2,
    TARGET_AUDIO    = 3,
    ANIM_TARGETS    = 4
} anim_targets;

static const u32 anim_target_ulen[ANIM_TARGETS] = {
    GFX_MAIN_LEN, GFX_MAIN_LEN, GFX_SUB_LEN, 44100
};

typedef struct {
    u32 magic; /* Magic 'BAXX' */
    u32 foff;  /* Start of stream offset */
    u32 size;  /* Size of file (including header) */
    u32 rate;  /* Stream framerate */
    /* rest is reserved */
} __attribute__((packed)) anim_header;

typedef struct {
    u32 magic; /* Magic 'FRME' */
    u32 clen;  /* Compressed stream length */
    u32 tgt;   /* Target ID */
    /* stream data follows here */
    u8  strm;
} __attribute__((packed)) anim_frame;

static inline bool anim_header_valid(anim_header *anim, u32 anim_size)
{
    if (anim->magic == ANIM_HEADER_MAGIC &&
        anim->size == anim_size &&
        anim->foff < anim_size)
        return true;
    return false;
}

static inline bool anim_frame_valid(anim_frame *frame)
{
    if (frame->magic == ANIM_FRAME_MAGIC &&
        frame->tgt < ANIM_TARGETS)
        return true;
    return false;
}
