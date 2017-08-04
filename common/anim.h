#pragma once
#include <common.h>

#define ANIM_HEADER_MAGIC ('B' | 'A'<<8 | 'X'<<16 | 'X'<<24)
#define ANIM_FRAME_MAGIC  ('F' | 'R'<<8 | 'M'<<16 | 'E'<<24)

#define ANIM_MAXRATE  (30)
#define ANIM_TARGETS  (3)

#define FRAME_MAXSIZE (400*240*2)

typedef struct {
    u32 magic; /* Magic 'BAXX' */
    u32 foff;  /* Start of stream offset */
    u32 size;  /* Size of file (including header) */
    u32 rate;  /* Stream framerate */
    /* rest is potentially reserved */
} anim_header;

typedef struct {
    u32 magic;       /* Magic 'FRME' */
    u32 crc;         /* Simple checksum (comp_size + uncomp_size + target) */
    u32 comp_size;   /* Size of compressed data (not including header) */
    u32 uncomp_size; /* Size of uncompressed data */
    u32 target;      /* Target ID */
    /* stream data follows here */
} anim_frame;

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
        frame->uncomp_size <= FRAME_MAXSIZE &&
        frame->comp_size < frame->uncomp_size &&
        frame->target <= ANIM_TARGETS &&
        ((frame->comp_size + frame->uncomp_size + frame->target) == frame->crc))
        return true;
    return false;
}
