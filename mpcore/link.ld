OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(__start)

MEMORY
{
  AXIRAM  (RWX) : ORIGIN = 0x1FF80000, LENGTH = 0x7E000
  FCRAM   (RW)  : ORIGIN = 0x20000000, LENGTH = 128M
}

SECTIONS
{
    .text : ALIGN(16)
    {
        __text_s = ABSOLUTE(.);
        *(.text.start)
        *(.text*)
        . = ALIGN(16);
        __text_e = ABSOLUTE(.);
    } >AXIRAM

    .data : ALIGN(16)
    {
        __data_s = ABSOLUTE(.);
        *(.data*)
        *(.rodata*)
        . = ALIGN(16);
        __data_e = ABSOLUTE(.);
    } >AXIRAM

    .bss : ALIGN(16)
    {
        __bss_s = ABSOLUTE(.);
        *(.bss*)
        . = ALIGN(16);
        __bss_e = ABSOLUTE(.);
    } >AXIRAM

    __heap_start = ORIGIN(FCRAM);
    __heap_end   = ORIGIN(FCRAM) + LENGTH(FCRAM);

    __stack_top = ORIGIN(AXIRAM) + LENGTH(AXIRAM);
    __stack_abt = __stack_top;
    __stack_svc = __stack_abt - 0x3000;

    __end__ = ABSOLUTE(.);
}
