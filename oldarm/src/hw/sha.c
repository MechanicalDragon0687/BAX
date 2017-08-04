#include <common.h>
#include <hw/sha.h>

static inline void sha_wait(void)
{
    u32 cnt;
    do {
        cnt = *SHA_CNT;
    } while(cnt & (SHA_START | SHA_FINAL));
    return;
}

void sha(u32 *hash, const u32 *data, u32 size, u32 mode)
{
    u32 hashlen;

    sha_wait();
    *SHA_CNT = SHA_START | SHA_ENDIAN | mode;
    while(size > 0) {
        if (size & 0x1F) {
            sha_wait();
        }
        *SHA_FIFO = *(data++);
        size-=4;
    }
    sha_wait();

    *SHA_CNT = (*SHA_CNT & ~SHA_START) | SHA_FINAL;
    sha_wait();

    switch(mode) {
    default:
    case SHA256:
        hashlen = 8;
        break;
    case SHA224:
        hashlen = 7;
        break;
    case SHA1:
        hashlen = 5;
        break;
    }

    for (u32 i = 0; i < hashlen; i++) {
        hash[i] = SHA_HASH[i];
    }
    return;
}
