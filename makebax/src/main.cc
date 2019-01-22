#include <iostream>
#include <fstream>
#include <cstring>
#include <cassert>
#include <mutex>
#include <stdexcept>

#include <omp.h>
#include <lz4.h>
#include <lz4hc.h>

#include "arg.h"
#include "bax.h"
#include "main.h"
#include "raw.h"

void DeltaEncode(uint16_t *bb, uint16_t *fb, unsigned sz)
{
    uint32_t *backbuffer, *framebuffer, delta;

    backbuffer  = reinterpret_cast<uint32_t*> (bb);
    framebuffer = reinterpret_cast<uint32_t*> (fb);
    sz *= sizeof(uint16_t);
    sz /= sizeof(uint32_t);

    for (unsigned i = 0; i < sz; i++) {
        delta = framebuffer[i] - backbuffer[i];
        backbuffer[i]  = framebuffer[i];
        framebuffer[i] = delta;
    }
    return;
}

void PrintUsage(void)
{
    std::cout << "makebax: [-i input.mp4] [-b background color] [-o output.bax] [-c lvl]" << std::endl;
}

int main(int argc, char *argv[])
{
    char *argv0;

    std::mutex out_lock;
    std::ofstream output_stream;
    std::string input_path, output_path;
    unsigned processed_frames, dimension, complevel;

    unsigned offset, length;
    uint16_t *backbuffer, bg;
    BAX::Header *main_header;
    RAW::Container *raw_frames;

    if (argc < 5) {
        PrintUsage();
        return EXIT_FAILURE;
    }

    bg = 0; // Default background color
    complevel = 6; // Good default
    ARGBEGIN {
        case 'i':
            input_path = EARGF(PrintUsage());
            break;

        case 'b':
            bg = strtol(EARGF(PrintUsage()), NULL, 0);
            break;

        case 'c':
            complevel = strtol(EARGF(PrintUsage()), NULL, 0);
            break;

        case 'o':
            output_path = EARGF(PrintUsage());
            break;

        default:
            PrintUsage();
            return EXIT_FAILURE;
    } ARGEND;

    std::cout << "makebax v" MAKEBAX_VERSION << std::endl;
    std::cout << "Using " << omp_get_max_threads() << " threads" << std::endl;

    output_stream = std::ofstream(output_path, std::ios::out | std::ios::binary);
    if (!output_stream.is_open()) {
        std::cout << "Failed to open " << output_path << std::endl;
        return EXIT_FAILURE;
    }

    // Decode frames from codec to raw RGB565
    raw_frames = new RAW::Container(input_path);
    if (raw_frames->Height() != 240) {
        std::cout << "Invalid height on input (" << raw_frames->Height() << ") (should be 240)" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Detected: ";

    if (raw_frames->Width() == 320) {
        offset = 400;
        length = 320;
        std::cout << "bottom screen only animation" << std::endl;
    } else if (raw_frames->Width() == 400) {
        offset = 0;
        length = 400;
        std::cout << "top screen only animation" << std::endl;
    } else if (raw_frames->Width() == 720) {
        offset = 0;
        length = 720;
        std::cout << "dual screen animation" << std::endl;
    } else {
        std::cout << "unknown - invalid video dimensions " << raw_frames->Width() << "x" << raw_frames->Height() <<
                    " (should be 400x240, 320x240 or 720x240)" << std::endl;
        return EXIT_FAILURE;
    }

    raw_frames->DumpInfo();
    dimension = raw_frames->Width() * raw_frames->Height();

    while(raw_frames->ProcessNextFrame());
    std::cout << "Read " << raw_frames->Frames() << " frames from " << input_path << std::endl;


    // Delta encoding
    /*
     * Can't be parallelized because of backbuffer consistency requirements
     * Best that can be done is split the fb into chunks and have each chunk
     * be processed in parallel, but that just completely thrashes the
     * level 2 cache on real systems.
     */
    backbuffer = new uint16_t[dimension] {0};
    for (unsigned i = 0; i < raw_frames->Frames(); i++) {
        DeltaEncode(backbuffer, raw_frames->Frame(i), dimension);
    }


    // Create header and initialize parameters
    main_header = new BAX::Header(offset, length);

    main_header->SetAuthor("Wolfvak");
    main_header->SetInfo("Testing makebax");
    main_header->SetBackgroundColor(bg);
    main_header->SetFramerate(raw_frames->Framerate());

    // Compression
    processed_frames = 0;

    #pragma omp parallel for
    for (unsigned i = 0; i < raw_frames->Frames(); i++) {
        BAX::Frame *frame = new BAX::Frame(raw_frames->Frame(i), dimension * sizeof(uint16_t), complevel);
        main_header->AddFrame(frame, i);

        out_lock.lock();
        processed_frames++;
        std::cout << "Compressed frame " << processed_frames << " / " << raw_frames->Frames() << "\r";
        out_lock.unlock();
    }

    main_header->DumpFile(output_stream);

    std::cout << std::endl;
    std::cout << "Wrote " << (main_header->TotalSize() / 1024) << " KiB to " << output_path << std::endl;

    delete main_header;
    delete raw_frames;
    delete[] backbuffer;
    return 0;
}
