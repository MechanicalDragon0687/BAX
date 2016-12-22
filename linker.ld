ENTRY(_main_)

SECTIONS
{
    . = 0x23F00000;

    .text.start : ALIGN(4) { *(.text.start) }
    .text       : ALIGN(4) { *(.text) }
    .rodata     : ALIGN(4) { *(.rodata) }
    .data       : ALIGN(4) { *(.data) }

    __bss_start = .;
    .bss        : ALIGN(4) { *(.bss COMMON) }
    __bss_end   = .;

    __end__ = .;

    __svc_stack_start = 0x08100000;
    __irq_stack_start = . + 0x4000;
    __abt_stack_start = . + 0x8000;
    __und_stack_start = . + 0xC000;

    . = . + 0xC000;

    ASSERT (. < 0x23FFFE00, "Error: ARM9 payload is too big senpai")
}
