OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(__boot)

MEMORY
{
  AWRAM (rwx) : ORIGIN = 0x1FF80000, LENGTH = 512K - 4
}

SECTIONS
{
    .text :
    {
        . = ALIGN(4);
        *(.text.boot)
        *(.text*)
        . = ALIGN(4);
    } >AWRAM

    .data :
    {
        . = ALIGN(4);
        *(.data*);
        *(.rodata*);
        *(COMMON);
    } >AWRAM

    _irq_stack = ORIGIN(AWRAM) + LENGTH(AWRAM);
    _prg_stack = _irq_stack - 0x400;
}
