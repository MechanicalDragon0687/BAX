ENTRY(_main_)

SECTIONS
{
    . = 0x1FF80000;

    .text.start : ALIGN(4) { *(.text.start) }
    .text       : ALIGN(4) { *(.text) }
    .rodata     : ALIGN(4) { *(.rodata) }
    .data       : ALIGN(4) { *(.data) }
    .bss        : ALIGN(4) { *(.bss COMMON) }

    __stack_start = 0x22000000;
    __end__ = 0x22000000;
}
