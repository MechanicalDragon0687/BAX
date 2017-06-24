#pragma once
#include <common.h>

#define TOP_FRAME_SIZE    (400*240*2)
#define BOTTOM_FRAME_SIZE (320*240*2)

#define BAX_HEADER_MAGIC  ('B' | 'A' << 8 | 'X' << 16 | '0' << 24)

#define BAX_TIMER         (DEFAULT_TIMER)

typedef struct
{
    uint32_t magic;
    uint32_t rate;
    uint32_t topcnt;
    uint32_t botcnt;
    uint8_t reserved[112];
} bax_header;

static inline bool bax_magic_valid(bax_header *hdr)
{
    return (hdr->magic == BAX_HEADER_MAGIC);
}

volatile bool *bax_start(void);
