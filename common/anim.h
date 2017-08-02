#pragma once
#include <common.h>

#define ANIM_HEADER_MAGIC ((u8[]){'B','A','X','X'})
#define ANIM_FRAME_MAGIC  ((u8[]){'F','R','M','E'})

typedef struct {
    u8 magic[4]; /* Magic 'BAXX' */
    u32 foff;    /* Start of stream offset */
    u32 size;    /* Size of file (including header) */
    u32 rate;
    /* rest is potentially reserved */
} anim_header;

typedef struct {
    u8 magic[4];     /* Magic 'FRME' */
    u32 comp_size;   /* Size of compressed data (not including header) */
    u32 uncomp_size; /* Size of uncompressed data */
    u32 target;      /* Target ID */
    /* stream data follows here */
} anim_frame;
