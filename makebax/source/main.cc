/**
Copyright 2018 Wolfvak
For more information, read LICENSE.
*/


#include <iostream>
#include <cstring>
#include <mutex>

#include <lz4.h>
#include <lz4hc.h>

#include "bax.h"
#include "ivf.h"
#include "main.h"

using namespace std;

#define MIN_BLOCKSIZE (2)
#define DEF_BLOCKSIZE (48)
#define MAX_BLOCKSIZE (1024)

/*
 Latest LZ4 does include minimum and maximum compression levels
 but this'll have to do (even buster/sid doesn't have them!)
*/
#define MIN_COMPLVL_LZ4 (1)
#define DEF_COMPLVL_LZ4 (6)
#define MAX_COMPLVL_LZ4 (16)


void DeltaEncode(uint16_t *bb, uint16_t *fb, int sz)
{
    uint32_t *backbuffer, *framebuffer, delta;

    backbuffer  = reinterpret_cast<uint32_t*> (bb);
    framebuffer = reinterpret_cast<uint32_t*> (fb);

    for (size_t i = 0; i < (sz / sizeof(uint32_t)); i++) {
        delta = framebuffer[i] - backbuffer[i];
        backbuffer[i]  = framebuffer[i];
        framebuffer[i] = delta;
    }
    return;
}

uint8_t *CompressFrame(uint16_t *fb, int sz, size_t &szc, int clvl)
{
    size_t max_comp_size, comp_size;
    uint8_t *bigblock, *retblock;

    max_comp_size = LZ4_compressBound(sz);
    bigblock = new uint8_t[max_comp_size];

    comp_size = LZ4_compress_HC(
        reinterpret_cast<const char*> (fb),
        reinterpret_cast<char*> (bigblock),
        sz, max_comp_size, clvl);

    if (comp_size <= 0)
        Abort_Error("Failed to compress data.\n");

    szc = comp_size;
    retblock = new uint8_t[comp_size];
    memcpy(retblock, bigblock, comp_size);
    delete []bigblock;

    return retblock;
}

int main(int argc, char *argv[])
{
    static_assert(sizeof(size_t) >= 4, "sizeof(size_t) < 4? Seriously?\n");

    BAX *BAX_State;
    IVF *IVF_State;
    mutex stdout_mutex;
    uint16_t *rgb_buffer, *backbuffer;
    int blocksize = -1, complvl = -1, background = 0, frames_rem, frames_proc = 0;
    const char *IVF_Path, *BAX_Path, *Author = NULL, *Info = NULL;

    printf("makebax v" MAKEBAX_VERSION "\n");
    if (argc < 3 || ((argc - 3) % 2 != 0)) {
        Abort_Error(
            "Usage: %s \"input.ivf\" \"output.bax\" [-s n] [-c l] "
            "[-b back_color] [-a \"Author\"] [-i \"Info\"]\n",
            argv[0]);
    }

    IVF_Path = argv[1];
    BAX_Path = argv[2];

    if (argc >= 5) {
        for (auto i = 3; i < argc; i++) {
            char opt_char;
            if (strlen(argv[i]) == 2 && argv[i][0] == '-') {
                opt_char = argv[i++][1];
                switch(opt_char) {
                    case 's':
                        blocksize = strtol(argv[i], nullptr, 0);
                        break;

                    case 'c':
                        complvl = strtol(argv[i], nullptr, 0);
                        break;

                    case 'b':
                        background = strtol(argv[i], nullptr, 0);
                        break;

                    case 'a':
                        Author = argv[i];
                        break;

                    case 'i':
                        Info = argv[i];
                        break;

                    default:
                        Abort_Error("Unknown option \"-%c\"\n", opt_char);
                }
            }
        }
    }

    // TODO move the very similar code to its own inline subroutine?
    if (blocksize == -1) blocksize = DEF_BLOCKSIZE;
    if (blocksize < MIN_BLOCKSIZE || blocksize > MAX_BLOCKSIZE) {
        cout << "Block size " << blocksize << " is out of bounds - using " << DEF_BLOCKSIZE << " instead.\n";
        blocksize = DEF_BLOCKSIZE;
    }

    if (complvl == -1) complvl = DEF_COMPLVL_LZ4;
    if (complvl < MIN_COMPLVL_LZ4 || complvl > MAX_COMPLVL_LZ4) {
        cout << "Compression level " << complvl << " is out of bounds - using " << DEF_COMPLVL_LZ4 << " instead.\n";
        complvl = DEF_COMPLVL_LZ4;
    }

    IVF_State = new IVF(IVF_Path);

    if (IVF_State->GetHeight() != FRAME_H)
        Abort_Error("Invalid height %d (expected %d).\n", IVF_State->GetHeight(), FRAME_H);

    BAX_State = new BAX(BAX_Path, IVF_State->GetFrameCount());

    if (IVF_State->GetWidth() == 400 || IVF_State->GetWidth() == 720) {
        BAX_State->SetAnimOffset(0);
    } else if (IVF_State->GetWidth() == 320) {
        BAX_State->SetAnimOffset(400);
    } else {
        Abort_Error("Invalid width %d.\n", IVF_State->GetWidth());
    }

    BAX_State->SetAnimWidth(IVF_State->GetWidth());
    BAX_State->SetAnimRate(IVF_State->GetFrameRate());
    BAX_State->SetBackgroundColor(background);

    if (Author != NULL) BAX_State->SetAuthor(Author);
    if (Info != NULL) BAX_State->SetInfo(Info);

    blocksize  = min(blocksize, static_cast<int> (IVF_State->GetFrameCount()));
    rgb_buffer = new uint16_t[IVF_State->GetFramePixels() * blocksize];
    backbuffer = new uint16_t[IVF_State->GetFramePixels()] {0};

    cout << "Working with a " << blocksize << " frame buffer.\n";
    cout << "Compression level " << complvl << ".\n";

    frames_rem = IVF_State->GetFrameCount();
    while(frames_rem > 0) {
        int iter_count = std::min(frames_rem, blocksize);
        uint8_t *CompressedBuffers[iter_count];

        // Decode VPX frames to RGB565
        for (auto i = 0; i < iter_count; i++) {
            IVF_State->DecodeNextFrame(&rgb_buffer[IVF_State->GetFramePixels() * i]);
        }

        // Delta encoding
        for (auto i = 0; i < iter_count; i++) {
            DeltaEncode(backbuffer, &rgb_buffer[IVF_State->GetFramePixels() * i], IVF_State->GetFrameByteSize());
        }

        // Compression
        #pragma omp parallel for
        for (auto i = 0; i < iter_count; i++) {
            uint8_t *CompressedBuffer;
            size_t CompressedSize;

            CompressedBuffer = CompressFrame(&rgb_buffer[IVF_State->GetFramePixels() * i],
                                             IVF_State->GetFrameByteSize(),
                                             CompressedSize,
                                             complvl);

            CompressedBuffers[i] = CompressedBuffer;

            BAX_State->AddFrame(CompressedBuffer, CompressedSize,
                                (IVF_State->GetFrameCount() - frames_rem + i));

            stdout_mutex.lock();
            frames_proc++;
            cout << "Frame " << frames_proc << " / " << IVF_State->GetFrameCount();
            cout << " (" << BAX_State->SizeInDisk() / 1024 << "KiB)\r" << flush;
            stdout_mutex.unlock();
        }

        BAX_State->FlushFramesToDisk();

        for (auto i = 0; i < iter_count; i++)
            delete []CompressedBuffers[i];

        frames_rem -= iter_count;
    }

    BAX_State->FlushHeaderToDisk();

    delete []backbuffer;
    delete []rgb_buffer;

    delete BAX_State;
    delete IVF_State;

    cout << "\n";
    return 0;
}
