/**
Copyright 2018 Wolfvak

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*/

#ifndef BAX_H
#define BAX_H

#include <iostream>
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

        uint32_t **FrameData;
        bool      *FramePresent;
        size_t     FlushStart, FlushEnd;

        std::mutex Lock;

    public:
        BAX(const char *path, size_t n);
        ~BAX(void);

        void SetAuthor(const char *auth);
        void SetInfo(const char *info);

        void SetBackgroundColor(size_t c);

        void SetAnimOffset(size_t o);
        void SetAnimWidth(size_t w);
        void SetAnimRate(size_t r);

        void AddFrame(uint32_t *f, size_t l, size_t p);

        void FlushFramesToDisk(void);

        size_t GetSizeInDisk(void);
        void   FlushHeaderToDisk(void);
};

#endif
