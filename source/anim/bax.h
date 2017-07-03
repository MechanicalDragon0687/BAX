#pragma once
#include <common.h>

#include <hw/hid.h>

#define TOP_FRAME_SIZE    (400*240*2)
#define BOTTOM_FRAME_SIZE (320*240*2)

#define BAX_HEADER_MAGIC  ('B' | 'A' << 8 | 'X' << 16 | '0' << 24)

#define MAX_RATE          (60)
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
    return ((hdr->magic == BAX_HEADER_MAGIC) && (hdr->rate <= MAX_RATE));
}

static inline bool abort_anim(void)
{
    return hid_key_pressed();
}

void bax_start(void);
