ENTRY(_main_)

SECTIONS
{
    . = 0x23F00000;

    .text.start : ALIGN(4) { *(.text.start) }
    .text       : ALIGN(4) { *(.text*) }
    .rodata     : ALIGN(4) { *(.rodata*) }
    .data       : ALIGN(4) { *(.data*) }

    .bss        : ALIGN(4)
    {
        __bss_start = .;
        *(.bss* COMMON)
        __bss_end   = .;
    }

    __sys_stack = 0x08100000; /* ARM9 internal memory */
    __abt_stack = 0x00008000; /* ITCM */
}