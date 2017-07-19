OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(__boot)

MEMORY
{
  AWRAM (rwx) : ORIGIN = 0x1FFFF800, LENGTH = 1K
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

    _stack_top = ORIGIN(AWRAM);
}
