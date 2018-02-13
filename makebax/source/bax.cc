/**
Copyright 2018 Wolfvak
For more information, read LICENSE.
*/


#include <assert.h>
#include <iostream>
#include <cstdint>

#include <mutex>

#include "bax.h"
#include "main.h"
#include "endian.h"

#define BAX_VERSION (1)

// Exactly 1092.2667 seconds @ 60fps
// I'd say that's more than enough
#define BAX_MAX_FRAMES (64 * 1024)

#define BAX_MAX_OFFSET (720)
#define BAX_MAX_WIDTH  (720)
#define BAX_MAX_RATE   (60)

static const uint8_t BAX_Signature[] = {'B', 'A', 'X', BAX_VERSION};

BAX::BAX(const char *path, size_t n)
{
    if (n > BAX_MAX_FRAMES)
        Abort_Error("Invalid BAX frame count %d.\n", n);

    this->File = fopen(path, "wb+");
    if (this->File == nullptr)
        Abort_Error("Couldn't open \"%s\" for writing.\n", path);

    this->FileSize = 256 + (8 * n);

    std::fill(this->Author, this->Author + 32, 0);
    std::fill(this->Info, this->Info + 192, 0);

    this->BackgroundColor = 0;

    this->AnimOffset = 0;
    this->AnimWidth  = 0;

    this->FrameCount = n;
    this->FrameRate  = 0;

    this->FrameStart = new size_t[n] {0};
    this->FrameSize  = new size_t[n] {0};

    this->FrameData  = new void*[n] {nullptr};

    this->FramePresent = new bool[n] {false};

    this->FlushStart = 0;
    this->FlushEnd   = 0;
    return;
}

BAX::~BAX(void)
{
    delete []this->FramePresent;
    delete []this->FrameData;

    delete []this->FrameStart;
    delete []this->FrameSize;

    fclose(this->File);
    return;
}

void BAX::AddFrame(void *f, size_t l, size_t p)
{
    std::lock_guard<std::mutex> lock_guard(this->Lock);

    assert(f != nullptr);
    assert(p < this->FrameCount);
    assert(this->FramePresent[p] == false);

    this->FrameStart[p] = this->FileSize;
    this->FrameSize[p]  = l;
    this->FrameData[p]  = f;

    this->FramePresent[p] = true;

    this->FileSize += Align_Up(l, 4);

    if (p < this->FlushStart) {
        this->FlushStart = p;
    } else if (p > this->FlushEnd) {
        this->FlushEnd = p;
    }
    return;
}

void BAX::FlushFramesToDisk(void)
{
    std::lock_guard<std::mutex> lock_guard(this->Lock);

    if (this->FlushStart == this->FlushEnd)
        return;

    for (auto i = this->FlushStart; i <= this->FlushEnd; i++) {
        if (this->FramePresent[i] == false)
            Abort_Error("Frame %d not present for writing!\n", i);

        fseek(this->File, this->FrameStart[i], SEEK_SET);
        if (fwrite(this->FrameData[i], this->FrameSize[i], 1, this->File) != 1)
            Abort_Error("Couldn't write frame %d (%d bytes) to output!\n", i, this->FrameSize[i]);
    }

    this->FlushEnd++;
    this->FlushStart = this->FlushEnd;
    return;
}

void BAX::FlushHeaderToDisk(void)
{
    /*
     - Main Header
    | Offset         | Length | Description |
    | :------------: | :----: | :---------- |
    | 0x00           | 0x03   | Header magic, always ASCII "BAX" |
    | 0x03           | 0x01   | Header version |
    | 0x04           | 0x04   | Animation special flags (detailed below) |
    | 0x08           | 0x04   | Frame count |
    | 0x0C           | 0x04   | Frame rate  |
    | 0x10           | 0x02   | Background color |
    | 0x12           | 0x06   | Reserved |
    | 0x18           | 0x04   | Column offset from start |
    | 0x1C           | 0x04   | Frame width |
    | 0x20           | 0x20   | ASCII author |
    | 0x40           | 0xC0   | ASCII description |
    | 0x100          | 0x08*n | Frame information |
    | 0x100 + 0x08*n | ???    | Compressed frames |

     - Frame information header
    | Offset | Length | Description |
    | :----: | :----: | :---------- |
    | 0x00   | 0x04   | Frame offset from start of file |
    | 0x04   | 0x04   | Compressed frame size |
    */

    std::lock_guard<std::mutex> lock_guard(this->Lock);

    const uint32_t HeaderData[] = {
        0,

        le32(static_cast<uint32_t> (this->FrameCount)),
        le32(static_cast<uint32_t> (this->FrameRate)),

        le32(static_cast<uint32_t> (this->BackgroundColor)),
        0,

        le32(static_cast<uint32_t> (this->AnimOffset)),
        le32(static_cast<uint32_t> (this->AnimWidth))
    };

    fseek(this->File, 0L, SEEK_SET);
    fwrite(BAX_Signature, sizeof(BAX_Signature), 1, this->File);

    fseek(this->File, 4L, SEEK_SET);
    fwrite(HeaderData, sizeof(HeaderData), 1, this->File);

    fseek(this->File, 32L, SEEK_SET);
    fwrite(this->Author, strnlen(this->Author, 32), 1, this->File);

    fseek(this->File, 64L, SEEK_SET);
    fwrite(this->Info, strnlen(this->Info, 192), 1, this->File);

    fseek(this->File, 256L, SEEK_SET);
    for (size_t i = 0; i < this->FrameCount; i++) {
        const uint32_t FrameInfo[] = {
            le32(static_cast<uint32_t> (this->FrameStart[i])),
            le32(static_cast<uint32_t> (this->FrameSize[i]))
        };

        fwrite(FrameInfo, sizeof(FrameInfo), 1, this->File);
    }
    return;
}
