#pragma once

#include <common.h>

// Multiply any x position by this to get the address offset
#define ANIM_WIDTH_MULT     (480)

// Arbitrary
#define MAX_ALLOC_ATTEMPTS  (262144)

// Animation header limits/definitions
#define ANIM_MAX_SIZE (0x2000000) // 32MB

#define ANIM_MAGIC    ((u8[]){'B', 'A', 'X'})

#define ANIM_MIN_VER  (0)
#define ANIM_MAX_VER  (0)

#define ANIM_MIN_FRAMES (1)
#define ANIM_MAX_FRAMES (8192)

#define ANIM_MIN_RATE (1)
#define ANIM_MAX_RATE (60)

#define ANIM_MIN_OFFSET (0)
#define ANIM_MAX_OFFSET (1120)

#define ANIM_MIN_LENGTH (1)
#define ANIM_MAX_LENGTH (1120)

#define ANIM_MIN_FSIZE  (1 * ANIM_WIDTH_MULT)
#define ANIM_MAX_FSIZE  (ANIM_MAX_LENGTH * ANIM_WIDTH_MULT)


// Animation error codes
enum
{
    ANIM_OK = 0,
    ANIM_ERR_MEM,
    ANIM_ERR_MAGIC,
    ANIM_ERR_VERSION,
    ANIM_ERR_FRAMECOUNT,
    ANIM_ERR_FRAMERATE,
    ANIM_ERR_OFFSET,
    ANIM_ERR_WIDTH,
    ANIM_ERR_SIZE,
    ANIM_ERR_INFO_OFF,
    ANIM_ERR_INFO_SIZE,
    ANIM_ERR_DECOMPRESSION,
    ANIM_ERR_UNKNOWN
};

// Animation special flags
enum
{
    ANIM_FLAG_STEREOSCOPIC = BIT(0)
};

typedef struct
{
    u32 offset;   // Offset to the compressed frame (from start of file)
    u32 compsz;   // Compressed frame size
} anim_finfo_t;

typedef struct
{
    u8  magic[3]; // 'BAX'
    u8  version;  // Animation header version
    u32 flags;    // Misc flags
    u32 frame_n;  // Number of frames in the animation
    u32 frame_r;  // Animation framerate

    u16 clear_c;  // Initial background color
    u8 resv[6];   // Reserved, should be zero

    u32 x_offset; // Frame X-offset
    u32 x_length; // Frame width

    char author[32];
    char description[192];

    anim_finfo_t frame_info[0];
    // Frame table comes after this
    // Compressed frames come after this
} anim_t;


// Get the location of a compressed frame
static inline const void *anim_frame_data(const anim_t *hdr, int frame)
{
    return (void*)((u8*)hdr + hdr->frame_info[frame].offset);
}

static inline u32 anim_frame_size(const anim_t *hdr, int frame)
{
    return hdr->frame_info[frame].compsz;
}

// Validate BAX animation
int anim_validate(const anim_t *hdr, size_t hdr_sz);

// Play BAX animation
void anim_play(const anim_t *hdr);
