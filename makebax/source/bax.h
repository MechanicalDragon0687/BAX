/**
Copyright 2018 Wolfvak
For more information, read LICENSE.
*/


#ifndef BAX_H
#define BAX_H

#include <iostream>
#include <cassert>
#include <cstring>
#include <mutex>

class BAX {
    private:
        FILE *File;

        char Author[32];
        char Info[192];

        size_t BackgroundColor;

        size_t FileSize;

        size_t AnimOffset;
        size_t AnimWidth;

        size_t FrameCount;
        size_t FrameRate;

        size_t *FrameStart;
        size_t *FrameSize;

        void **FrameData;
        bool  *FramePresent;
        size_t FlushStart, FlushEnd;

        std::mutex Lock;

    public:
        BAX(const char *path, size_t n);
        ~BAX(void);

        void SetAuthor(const char *auth) {
            assert(auth != NULL);
            strncpy(this->Author, auth, 31);
        }

        void SetInfo(const char *info) {
            assert(info != NULL);
            strncpy(this->Info, info, 191);
        }

        void SetBackgroundColor(size_t c) {
            this->BackgroundColor = static_cast<uint16_t> (c);
        }

        void SetAnimOffset(size_t o) {
            this->AnimOffset = o;
        }

        void SetAnimWidth(size_t w) {
            this->AnimWidth = w;
        }

        void SetAnimRate(size_t r) {
            this->FrameRate = r;
        }

        size_t SizeInDisk(void) {
            return this->FileSize;
        }

        void AddFrame(void *f, size_t l, size_t p);

        void FlushFramesToDisk(void);

        void FlushHeaderToDisk(void);
};

#endif
