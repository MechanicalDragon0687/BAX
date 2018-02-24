#include <types.h>
#include "hw/sha.h"

#define REG_SHA_CNT    (*(vu32*)(0x1000A000))
#define REG_SHA_BLKCNT (*(vu32*)(0x1000A004))

#define REG_SHA_HASH   ((u32*)(0x1000A040))
#define REG_SHA_FIFO   ((u32*)(0x1000A080))

static inline void SHA_Wait(void) {
    while(REG_SHA_CNT & (SHA_START | SHA_FINAL));
}

void SHA_Init(int mode)
{
    SHA_Wait();
    mode &= SHA_MODE_MASK;
    REG_SHA_CNT = SHA_START | SHA_BIGENDIAN | mode;
}

void SHA_UpdateBlock(const u32 *data, u32 len)
{
    len /= sizeof(u32);
    while(len--) {
        SHA_Wait();
        *REG_SHA_FIFO = *data++;
    }
}

void SHA_RetrieveHash(u32 *hash)
{
    u32 hash_sz = SHA_HashSizeWord(SHA_MODE_MASK & SHA_MODE_MASK);
    REG_SHA_CNT = (REG_SHA_CNT & ~SHA_START) | SHA_FINAL;
    SHA_Wait();

    for (u32 i = 0; i < hash_sz; i++)
        hash[i] = REG_SHA_HASH[i];
}

void SHA_CalculateHash(const u32 *data, u32 len, int mode, u32 *hash)
{
    SHA_Init(mode);
    SHA_UpdateBlock(data, len);
    SHA_RetrieveHash(hash);
}
