#ifndef IVF_H
#define IVF_H

#include <vpx/vpx_decoder.h>

#include "endian.h"

typedef struct IVF_Header {
    uint8_t  signature[4];
    uint16_t version;
    uint16_t header_len;
    uint32_t fourcc;
    uint16_t width;
    uint16_t height;
    uint32_t rate;
    uint32_t scale;
    uint32_t count;
    uint8_t  dummy[4];
} __attribute__((packed)) IVF_Header;

typedef struct IVF_Frame_Header {
    uint32_t size;
    uint64_t stamp;
    uint8_t  data[0];
} __attribute__((packed)) IVF_Frame_Header;

typedef struct VPX_State {
    size_t   ivf_size;
    uint8_t *ivf_buffer;

    IVF_Header *hdr;
    IVF_Frame_Header *frame;

    vpx_codec_ctx_t  codec;
} VPX_State;

static inline size_t VPX_Width(VPX_State *vpx)
{
    return le16(vpx->hdr->width);
}

static inline size_t VPX_Height(VPX_State *vpx)
{
    return le16(vpx->hdr->height);
}

static inline size_t VPX_Framerate(VPX_State *vpx)
{
    return le32(vpx->hdr->rate);
}

static inline size_t VPX_Framecount(VPX_State *vpx)
{
    return le32(vpx->hdr->count);
}

static inline size_t IVF_Headerlen(VPX_State *vpx)
{
    return le16(vpx->hdr->header_len);
}

static inline size_t IVF_FrameSize(VPX_State *vpx)
{
    return le32(vpx->frame->size);
}

VPX_State *VPX_Init(const char *path);
void VPX_Kill(VPX_State *vpx);
uint8_t **VPX_DecodeFrame(VPX_State *vpx, size_t *size, int *count);

#endif
