#include <stddef.h>
#include <stdint.h>

#include <arm/asm.h>

#include <io/term.h>
#include <io/printf.h>

static console bottom_out;

void __attribute__((noreturn)) print_registers(uint32_t *regs)
{ // order of registers is r0-r15, cpsr (pre-exception), cpsr (post-exception)

    term_init(&bottom_out, 0, ~0, BOTTOM_SCREEN);

    printf("BOOM!\n");

    for (int i = 0; i < 16; i++)
    {
        bool c = !(i % 2);
        printf(" R%02d = 0x%08X %c", i, regs[i], c ? '|' : '\n');
        // formatting
    }

    printf("SPSR = 0x%08X | CPSR = 0x%08X\n", regs[16], regs[17]);
    printf("Entering WFI state\n");

    wfi();
    printf("u wot");

    while(1) wfi();
}
