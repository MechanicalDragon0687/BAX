#include <stddef.h>
#include <stdint.h>

#include <arm/asm.h>
#include <io/term.h>
#include <io/printf.h>

static console bottom_out;

void __attribute__((noreturn, target("arm"))) print_registers(uint32_t *regs)
{
    // order of registers is r0-r15, cpsr (pre-exception), cpsr (post-exception)
    // TODO: Make sure framebuffers are set up

    term_init(&bottom_out, ~0, BOTTOM_SCREEN);
    printf("exception\n\n");
 
    for (int i = 0; i < 16; i++)
        printf(" r%02d = 0x%08X %c", i, regs[i], !(i % 2) ? '|' : '\n');
    // formatting

    printf("CPSR = 0x%08X | EPSR = 0x%08X\n", regs[16], regs[17]);
    while(1) wfi();
}
