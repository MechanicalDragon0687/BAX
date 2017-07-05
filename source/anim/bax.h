#pragma once
#include <common.h>

#include <hw/hid.h>

#define TOP_FRAME_SIZE    (400*240*2)
#define BOTTOM_FRAME_SIZE (320*240*2)

#define BAX_HEADER_MAGIC  ('B' | 'A' << 8 | 'X' << 16 | '0' << 24)

#define MAX_RATE          (60)
#define MAX_FRAMES        (1800)
/*
 arbitrary, max anim size is ((TOP_FRAME_SIZE + BOTTOM_FRAME_SIZE)*MAX_FRAMES + sizeof(bax_header))
 currently about 600MiB + 128 bytes
*/

#define BAX_TIMER         (DEFAULT_TIMER)

typedef struct {
    uint32_t magic;
    uint16_t rate;
    uint16_t version;
    uint32_t topcnt;
    uint32_t botcnt;
    uint8_t reserved[112];
} bax_header;

static inline bool bax_hdr_valid(bax_header *hdr)
{
    return ((hdr->magic == BAX_HEADER_MAGIC) && (hdr->rate <= MAX_RATE) &&
            (hdr->topcnt <= MAX_FRAMES) && (hdr->botcnt <= MAX_FRAMES));
}

static inline bool bax_abort(void)
{
    return hid_key_pressed();
}

void bax_start(void);
