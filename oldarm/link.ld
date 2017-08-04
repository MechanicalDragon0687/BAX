OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(__boot)

MEMORY
{
  RAM (rwx) : ORIGIN = 0x08006000, LENGTH = 0x7A000
}

SECTIONS
{
    .boot : {
        *(.boot*);
        . = ALIGN(4);
    } > RAM

    .text : {
        *(.text*);
        . = ALIGN(4);
    } > RAM

    .rodata : {
        *(.rodata*);
        . = ALIGN(4);
    } > RAM

    .data : {
        *(.data*);
        . = ALIGN(4);
    } > RAM

    .bss (NOLOAD) : {
        __bss_start = .;
        *(.bss* COMMON);
        __bss_end = .;
        __bss_size = __bss_end - __bss_start;
        . = ALIGN(4);
    } > RAM

    _irq_stack = 0x08080000;
    _prg_stack = _irq_stack - 0x400;

    __end__ = .;
}
