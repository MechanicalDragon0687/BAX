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

#define ALIGNV(x)    __attribute__((aligned((x))))
#define UNUSED       __attribute__((unused))
#define NORETURN     __attribute__((noreturn))
#define NOINLINE     __attribute__((noinline))
#define PACKED       __attribute__((packed))

#define asmv         __asm__ __volatile__

#define assert(x)    do{if((x)==false){asmv("bkpt 0xFF\n\t");__builtin_unreachable();}}while(0)

#define bound(x,a,b) (((x) >= (a)) && (((x) <= (b))))

#define BAX_PATH "sdmc:/bax"
#define BAX_FILE ".bax"
#define BAX_FIRM BAX_PATH"/boot.firm"

#endif // __ASSEMBLER__


#define EXTENDET(x)  ((x) << 24 | (x) << 16 | (x) << 8 | (x))
#define EXTENDST(x)  ((x) << 16 | (x))
#define BIT(x)       (1<<(x))

#endif // COMMON_H