#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "endian.h"

#define BAX_MAGIC   ((uint8_t[]){'B', 'A', 'X'})
#define BAX_VERSION ((uint8_t)(1))

typedef struct BAX_FrameInfo
{
    uint32_t offset; // Offset to the compressed frame (from start of file)
    int32_t  compsz; // Compressed frame size
} __attribute__((packed)) BAX_FrameInfo;

typedef struct BAX_Header
{
    uint8_t  signature[3];
    uint8_t  version;
    uint32_t flags;
    int32_t  frame_n;
    int32_t  frame_r;

    uint16_t clear_c;
    uint8_t  resv[6];

    int32_t  x_offset;
    int32_t  x_width;

    char     author[32];
    char     description[192];
} __attribute__((packed)) BAX_Header;

BAX_Header *BAX_Init(int count, int rate, uint16_t bg, int off, int width, const char *auth, const char *desc)
{
    BAX_Header *ret = malloc(sizeof(BAX_Header));
    assert(ret != NULL);
    assert(sizeof(ret->signature) == sizeof(BAX_MAGIC));

    if (auth == NULL) auth = "Unknown";
    if (desc == NULL) desc = "Unknown";

    memset(ret, 0, sizeof(BAX_Header));

    memcpy(ret->signature, BAX_MAGIC, sizeof(BAX_MAGIC));
    ret->version = BAX_VERSION;

    ret->frame_n = le32(count);
    ret->frame_r = le32(rate);

    ret->clear_c = le16(bg);

    ret->x_offset = le32(off);
    ret->x_width  = le32(width);

    strncpy(ret->author, auth, sizeof(ret->author) - 1);
    strncpy(ret->description, desc, sizeof(ret->description) - 1);
    return ret;    
}

void BAX_Kill(BAX_Header *hdr)
{
    free(hdr);
    return;
}

BAX_FrameInfo *BAX_FrameInfoInit(BAX_Header *hdr)
{
    BAX_FrameInfo *ret = malloc(sizeof(BAX_FrameInfo) * hdr->frame_n);
    assert(ret != NULL);

    memset(ret, 0, sizeof(BAX_FrameInfo) * hdr->frame_n);
    return ret;
}

void BAX_FrameInfoSet(BAX_FrameInfo *info, int frame, size_t offset, size_t size)
{
    info[frame].offset = offset;
    info[frame].compsz = size;
    return;
}

void BAX_FrameInfoKill(BAX_FrameInfo *info)
{
    free(info);
}

size_t BAX_DataStart(BAX_Header *hdr)
{
    return (sizeof(BAX_Header) + sizeof(BAX_FrameInfo) * hdr->frame_n);
}

size_t BAX_HeaderSize(void)
{
    return sizeof(BAX_Header);
}

size_t BAX_FrameInfoSize(BAX_Header *hdr)
{
    return (sizeof(BAX_FrameInfo) * hdr->frame_n);
}
