#ifndef SHA_H
#define SHA_H

#include <common.h>

#define SHA_START (BIT(0))
#define SHA_FINAL (BIT(1))

#define SHA_IRQ   (BIT(2))

#define SHA_BIGENDIAN (BIT(3))

#define SHA_MODE_256  (0 << 4)
#define SHA_MODE_224  (1 << 4)
#define SHA_MODE_160  (2 << 4)
#define SHA_MODE_MASK (3 << 4)

#define SHA_ENABLE_FIFO (BIT(9))

static inline size_t SHA_HashSizeWord(int mode) {
    switch(mode) {
        default:
        case SHA_MODE_256: return 8;
        case SHA_MODE_224: return 7;
        case SHA_MODE_160: return 5;
    }
}

void SHA_Init(int mode);
void SHA_UpdateBlock(const u32 *data, u32 len);
void SHA_RetrieveHash(u32 *hash);

void SHA_CalculateHash(const u32 *data, u32 len, int mode, u32 *hash);

#endif // SHA_H
