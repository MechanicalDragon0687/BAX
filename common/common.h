#ifndef COMMON_H
#define COMMON_H

#ifndef ARM9
#ifndef ARM11
#error "Undefined processor!"
#endif // ARM11
#endif // ARM9

#ifndef __ASSEMBLER__

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <types.h>

#define asmv __asm__ __volatile__

static inline void assert(bool cond) {
    if (cond == false) {
        asmv("bkpt\n\t");
        __builtin_unreachable();
    }
}

#define bound(x,a,b) (((x) >= (a)) && (((x) <= (b))))

#define BAX_PATH "sdmc:/bax"
#define BAX_FILE ".bax"

#define BAX_FIRMSRCP BAX_PATH"/boot.txt"
#define BAX_FIRMFILE BAX_PATH"/boot.firm"

#endif // __ASSEMBLER__

#define EXTENDET(x) ((x) << 24 | (x) << 16 | (x) << 8 | (x))
#define EXTENDST(x) ((x) << 16 | (x))
#define BIT(x)      (1<<(x))

#endif // COMMON_H
