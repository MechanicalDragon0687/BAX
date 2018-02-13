/**
Copyright 2018 Wolfvak

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*/

#include <assert.h>
#include <iostream>
#include <cstring>

#define VPX_CODEC_DISABLE_COMPAT 1
#include <vpx/vp8dx.h>
#include <vpx/vpx_image.h>

#include "ivf.h"
#include "main.h"

#define IVF_HEADER_SIZE (32)

const uint8_t IVF_Signature[] = {'D', 'K', 'I', 'F'};

struct {
    const uint8_t FourCC[4];
    vpx_codec_iface_t *DX;
} VPX_Codecs[] = {
    {{'V', 'P', '8', '0'}, vpx_codec_vp8_dx()},
    {{'V', 'P', '9', '0'}, vpx_codec_vp9_dx()}
};
const int CodecCount = sizeof(VPX_Codecs) / sizeof(*VPX_Codecs);

IVF::IVF(const char *path)
{
    /*
     IVF Header
    bytes 0-3    signature: 'DKIF'
    bytes 4-5    version (should be 0)
    bytes 6-7    length of header in bytes
    bytes 8-11   codec FourCC (e.g., 'VP80')
    bytes 12-13  width in pixels
    bytes 14-15  height in pixels
    bytes 16-19  frame rate
    bytes 20-23  time scale
    bytes 24-27  number of frames in file
    bytes 28-31  unused
    */

    FILE *IVF_File;
    uint16_t HeaderLen, Version;
    uint8_t IVF_Header[IVF_HEADER_SIZE];
    vpx_codec_iface_t *DXInterface = nullptr;

    IVF_File = fopen(path, "rb+");
    if (IVF_File == nullptr)
        Abort_Error("Couldn't open \"%s\" for reading.\n", path);

    if (fread(IVF_Header, IVF_HEADER_SIZE, 1, IVF_File) != 1)
        Abort_Error("Couldn't read \"%s\".\n", path);

    if (memcmp(IVF_Header, IVF_Signature, sizeof(IVF_Signature)))
        Abort_Error("Not a valid IVF file (bad signature).\n");

    Version = le16(*(uint16_t*)(IVF_Header + 4));
    if (Version != 0)
        Abort_Error("Not a valid IVF file (bad version).\n");

    HeaderLen = le16(*(uint16_t*)(IVF_Header + 6));
    if (HeaderLen != IVF_HEADER_SIZE)
        Abort_Error("Not a valid IVF file (invalid header length).\n");

    this->Width  = le16(*(uint16_t*)(IVF_Header + 12));
    this->Height = le16(*(uint16_t*)(IVF_Header + 14));

    this->FrameRate  = le32(*(uint32_t*)(IVF_Header + 16));
    this->FrameCount = le32(*(uint32_t*)(IVF_Header + 24));

    this->Frames    = new uint8_t*[this->FrameCount] {nullptr};
    this->FrameSize = new size_t[this->FrameCount]   {0};

    this->WorkFrame = 0;

    for (auto i = 0; i < CodecCount; i++) {
        if (memcmp(IVF_Header + 8, VPX_Codecs[i].FourCC, 4) == 0)
            DXInterface = VPX_Codecs[i].DX;
    }

    if (DXInterface == nullptr)
        Abort_Error("Unknown codec ('%.4s').\n", IVF_Header + 8);

    if (vpx_codec_dec_init(&this->VPXCodec, DXInterface, nullptr, 0) != 0)
        Abort_Error("Failed to initialize codec ('%.4s').\n", IVF_Header + 8);

    fseek(IVF_File, IVF_HEADER_SIZE, SEEK_SET);
    for (size_t i = 0; i < this->FrameCount; i++) {
        /*
         IVF Frame Header
        bytes 0-3    size of frame in bytes (not including the 12-byte header)
        bytes 4-11   64-bit presentation timestamp
        bytes 12..   frame data
        */
        uint32_t CurrentFrameSize;

        if (fread(&CurrentFrameSize, sizeof(uint32_t), 1, IVF_File) != 1)
            Abort_Error("Failed to read the size of IVF frame %d.\n", i);

        CurrentFrameSize = le32(CurrentFrameSize);

        this->FrameSize[i] = CurrentFrameSize;
        this->Frames[i]    = new uint8_t[CurrentFrameSize];

        fseek(IVF_File, sizeof(uint64_t), SEEK_CUR);
        if (fread(this->Frames[i], CurrentFrameSize, 1, IVF_File) != 1)
            Abort_Error("Failed to read IVF frame %d (%d bytes).\n", i, CurrentFrameSize);
    }

    fclose(IVF_File);
    return;
}

IVF::~IVF(void)
{
    vpx_codec_destroy(&this->VPXCodec);
    for (size_t i = 0; i < this->FrameCount; i++)
        delete []this->Frames[i];

    delete []this->Frames;
    delete []this->FrameSize;
    return;
}

static inline int ClampByte(int n)
{
    if (n > 0xFF) return 0xFF;
    else if (n < 0) return 0;
    else return n;
}
static inline uint16_t YUV420ToRGB565Color(int y, int u, int v)
{
    int r, g, b;

    v -= 128;
    u -= 128;

    r = ClampByte(y + (1.370705 * v));
    r = (r >> 3) & 0x1F;

    g = ClampByte(y - (0.698001 * v) - (0.337633 * u));
    g = (g >> 2) & 0x3F;

    b = ClampByte(y + (1.732446 * u));
    b = (b >> 3) & 0x1F;

    return le16((r << 11) | (g << 5) | b);
}

static inline void IVF_ColorTrans(uint16_t *buffer, int w, int h,
                                  uint8_t *const *yuv, const int *strides)
{
    int yc, uc, vc;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {

            yc = yuv[0][(y * strides[0]) + x];
            uc = yuv[1][(y / 2) * strides[1] + (x / 2)];
            vc = yuv[2][(y / 2) * strides[2] + (x / 2)];

            buffer[(x * h) + (h - 1 - y)] = YUV420ToRGB565Color(yc, uc, vc);
        }
    }
    return;
}

int IVF::DecodeNextFrame(uint16_t *b)
{
    vpx_codec_iter_t iter = nullptr;
    vpx_image_t      *img = nullptr;
    vpx_codec_err_t   err;

    uint8_t *yuv[3];
    int f = this->WorkFrame, strides[3], ret = this->WorkFrame;

    assert(b != nullptr);

    err = vpx_codec_decode(&this->VPXCodec, this->Frames[f], this->FrameSize[f], nullptr, 0);

    if (err != VPX_CODEC_OK)
        Abort_Error("Failed to decode IVF frame %d (error %d).\n", f, err);

    img = vpx_codec_get_frame(&this->VPXCodec, &iter);
    if (img == nullptr)
        Abort_Error("Failed to retrieve IVF frame %d.\n", f);

    if (img->d_w != this->Width)
        Abort_Error("Bad frame width on IVF frame %d (got %d, expected %d).\n",
                    f, img->d_w, this->Width);

    if (img->d_h != this->Height)
        Abort_Error("Bad frame height on IVF frame %d (got %d, expected %d).\n",
                    f, img->d_h, this->Height);

    yuv[0] = img->planes[VPX_PLANE_Y];
    yuv[1] = img->planes[VPX_PLANE_U];
    yuv[2] = img->planes[VPX_PLANE_V];

    strides[0] = img->stride[VPX_PLANE_Y];
    strides[1] = img->stride[VPX_PLANE_U];
    strides[2] = img->stride[VPX_PLANE_V];

    IVF_ColorTrans(b, img->d_w, img->d_h, yuv, strides);

    this->WorkFrame++;
    vpx_img_free(img);
    return ret;
}
