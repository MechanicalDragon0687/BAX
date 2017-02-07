#include <common.h>

#include <arm/asm.h>
#include <io/term.h>
#include <io/printf.h>

void __attribute__((noreturn, target("arm"))) print_registers(u32 *regs)
{
    // order of registers is r0-r15, cpsr (pre-exception), cpsr (post-exception)
    // TODO: Make sure framebuffers are set up

    console bottom_out;
    term_init(&bottom_out, ~0, BOTTOM_SCREEN);
    printf("exception\n\n");

    for (int i = 0; i < 16; i++)
        printf(" r%02d = 0x%08X %c", i, regs[i], !(i % 2) ? '|' : '\n');
    // formatting

    printf("CPSR = 0x%08X | EPSR = 0x%08X\n", regs[16], regs[17]);
    while(1) wfi();
}
