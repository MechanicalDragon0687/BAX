#include <asm.h>
#include <mmap.h>

@ void BAX_DeltaDecode(void *bb, void *fb, size_t l)
ASM_FUNCTION BAX_DeltaDecode
    push {r4-r8, lr}

    1:
        ldmia r0, {r3, r4, r5, r6}
        ldmia r1, {r7, r8, r12, lr}

        subs r2, r2, #16
        popmi {r4-r8, pc}

        add r3, r3, r7
        add r4, r4, r8
        add r5, r5, r12
        add r6, r6, lr

        stmia r0!, {r3, r4, r5, r6}
        stmia r1!, {r3, r4, r5, r6}
        b 1b
