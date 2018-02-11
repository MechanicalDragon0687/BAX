#include <assert.h>

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define VPX_CODEC_DISABLE_COMPAT 1
#include <vpx/vp8dx.h>
#include <vpx/vpx_image.h>

#include "endian.h"
#include "vpx.h"

#define IVF_SIGNATURE ((uint8_t[]){'D', 'K', 'I', 'F'})

VPX_State *VPX_Init(const char *path)
{
    VPX_State *ret = NULL;
    size_t ivf_size;
    int res;
    FILE *ivf_file = fopen(path, "rb+");
    if (ivf_file == NULL)
        return NULL;

    ret = malloc(sizeof(VPX_State));
    assert(ret != NULL);
    memset(ret, 0, sizeof(VPX_State));

    fseek(ivf_file, 0L, SEEK_END);
    ivf_size = ftell(ivf_file);
    fseek(ivf_file, 0L, SEEK_SET);

    ret->ivf_size = ivf_size;
    ret->ivf_buffer = malloc(ivf_size);
    assert(ret->ivf_buffer != NULL);

    res = fread(ret->ivf_buffer, ivf_size, 1, ivf_file);
    fclose(ivf_file);

    if (res != 1) {
        free(ret->ivf_buffer);
        free(ret);
        return NULL;
    }

    if (memcmp(ret->ivf_buffer, IVF_SIGNATURE, sizeof(IVF_SIGNATURE))) {
        free(ret->ivf_buffer);
        free(ret);
        return NULL;
    }

    if (vpx_codec_dec_init(&ret->codec, vpx_codec_vp8_dx(), NULL, 0) != 0) {
        free(ret->ivf_buffer);
        free(ret);
        return NULL;
    }

    ret->hdr = (IVF_Header*)ret->ivf_buffer;
    ret->frame = (IVF_Frame_Header*)(ret->ivf_buffer + IVF_Headerlen(ret));

    return ret;
}

static inline uint8_t clamp8(int n)
{
    if (n < 0) return 0;
    else if (n > 0xFF) return 0xFF;
    else return n;
}

static inline uint16_t yuvtorgb_color(int y, int u, int v)
{
    int r, g, b;

    r = (clamp8(y + (1.370705 * (v-128))) >> 3) & 0x1F;
    g = (clamp8(y - (0.698001 * (v-128)) - (0.337633 * (u-128))) >> 2) & 0x3F;
    b = (clamp8(y + (1.732446 * (u-128))) >> 3) & 0x1F;

    return le16((r << 11) | (g << 5) | b);
}

static inline void yuv420torgb565(uint16_t width, uint16_t height,
                 const uint8_t *yb, const uint8_t *ub, const uint8_t *vb,
                 size_t ystride, size_t ustride, size_t vstride, 
                 uint16_t *out)
{
    int yc, uc, vc;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            int yc = yb[(y * ystride) + x];
            int uc = ub[(y / 2) * ustride + x / 2];
            int vc = vb[(y / 2) * vstride + x / 2];

            out[y*width + x] = yuvtorgb_color(yc, uc, vc);
        }
    }
}

uint8_t **VPX_DecodeFrame(VPX_State *vpx, size_t *size, int *count)
{
    uint8_t **ret = NULL, *buf;
    size_t sz = 0;
    int fcount = 0;

    vpx_codec_iter_t iter = NULL;
    vpx_image_t      *img = NULL;

    if (vpx_codec_decode(&vpx->codec, vpx->frame->data, IVF_FrameSize(vpx), NULL, 0) != 0)
        return NULL;

    while ((img = vpx_codec_get_frame(&vpx->codec, &iter)) != NULL) {
        fcount++;

        ret = realloc(ret, fcount * sizeof(char*));
        assert(ret != NULL);

        buf = malloc(img->d_w * img->d_h * 2);
        assert(buf != NULL);

        ret[fcount - 1] = buf;

        yuv420torgb565(
            img->d_w, img->d_h,
            img->planes[VPX_PLANE_Y], img->planes[VPX_PLANE_U], img->planes[VPX_PLANE_V],
            img->stride[VPX_PLANE_Y], img->stride[VPX_PLANE_U], img->stride[VPX_PLANE_V],
            (uint16_t*)buf);

        if (sz == 0)
            sz = img->d_w * img->d_h * 2;
    }

    if (count) *count = fcount;
    if (size) *size = sz;

    vpx->frame = (IVF_Frame_Header*)((uint8_t*)vpx->frame + vpx->frame->size + sizeof(IVF_Frame_Header));
    vpx_img_free(img);
    return ret;
}

void VPX_Kill(VPX_State *vpx)
{
    assert(vpx != NULL);

    free(vpx->ivf_buffer);
    vpx_codec_destroy(&vpx->codec);
    free(vpx);
    return;
}
