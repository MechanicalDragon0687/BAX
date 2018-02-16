OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(__start)

MEMORY
{
  AHBRAM  (RWX) : ORIGIN = 0x08006000, LENGTH = 0xFA000
  VECTORS (RWX) : ORIGIN = 0x00000000, LENGTH = 0x200
  ITCM    (RWX) : ORIGIN = 0x00000200, LENGTH = 0x3500
  DTCM    (RW)  : ORIGIN = 0x30000000, LENGTH = 0x4000
}

SECTIONS
{
    .bootstrap : ALIGN(16)
    {
        KEEP(*(.bootstrap))
        . = ALIGN(16);
    } >AHBRAM

    .text : ALIGN(16)
    {
        __text_lma = LOADADDR(.text);
        __text_s = ABSOLUTE(.);
        *(.text*)
        . = ALIGN(16);
        __text_e = ABSOLUTE(.);
    } >ITCM AT>AHBRAM

    .vectors : ALIGN(16)
    {
        __vectors_lma = LOADADDR(.vectors);
        __vectors_s = ABSOLUTE(.);
        KEEP(*(.vectors))
        . = ALIGN(16);
        __vectors_e = ABSOLUTE(.);
    } >VECTORS AT>AHBRAM

    .data : ALIGN(16)
    {
        __data_s = ABSOLUTE(.);
        *(.data*)
        *(.rodata*)
        . = ALIGN(16);
        __data_e = ABSOLUTE(.);
    } >AHBRAM

    .bss (NOLOAD) : ALIGN(16)
    {
        __bss_s = ABSOLUTE(.);
        *(.bss*)
        . = ALIGN(16);
        __bss_e = ABSOLUTE(.);
    } >AHBRAM

    __end__ = ABSOLUTE(.);

    __stack_irq = ORIGIN(AHBRAM) + LENGTH(AHBRAM);
    __stack_sys = __stack_irq - 0x2000;
}
