#ifndef ANIM_H
#define ANIM_H

#include <common.h>
#include "lib/fs/fs.h"

// Multiply any x position by this to get the address offset
#define ANIM_WIDTH_MULT     (240 * 2)

// Arbitrary
#define MAX_ALLOC_ATTEMPTS  (1024)

// Animation header limits/definitions
#define ANIM_MAX_SIZE   (0x7000000) // 112MiB

#define ANIM_MAGIC      ((u8[]){'B', 'A', 'X'})

#define ANIM_MIN_VER    (1)
#define ANIM_MAX_VER    (1)

#define ANIM_MIN_FRAMES (1)
#define ANIM_MAX_FRAMES (8192)

#define ANIM_MIN_RATE   (1)
#define ANIM_MAX_RATE   (60)

#define ANIM_MIN_OFFSET (0)
#define ANIM_MAX_OFFSET (719)

#define ANIM_MIN_WIDTH  (1)
#define ANIM_MAX_WIDTH  (720)

#define ANIM_MIN_FSIZE  (ANIM_MIN_WIDTH * ANIM_WIDTH_MULT)
#define ANIM_MAX_FSIZE  (ANIM_MAX_WIDTH * ANIM_WIDTH_MULT)


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
    ANIM_FLAG_NONE = BIT(0)
};

typedef struct
{
    u32 offset;   // Offset to the compressed frame (from start of file)
    u32 compsz;   // Compressed frame size
} BAX_FData;

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
    u32 x_width;  // Frame width

    char author[32];
    char description[192];
    // Frame table comes after this
    // Compressed frames come after that
} BAX;

static inline u32 BAX_FDataPos(BAX_FData *d) {
    return d->offset;
}

static inline u32 BAX_FDataCompSZ(BAX_FData *d) {
    return d->compsz;
}

void BAX_DeltaDecode(void *bb, void *fb, size_t l);

// Play BAX file
void BAX_Play(FS_File *bax_f, u32 skip_hid);

#endif // ANIM_H
