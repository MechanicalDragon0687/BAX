OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(__boot)

MEMORY
{
  itcm (rwx)    : ORIGIN = 0x00000000, LENGTH = 32K
  dtcm (rw)     : ORIGIN = 0x28008000, LENGTH = 16K
  ram (rwx)     : ORIGIN = 0x20000000, LENGTH = 128M
  fastram (rwx) : ORIGIN = 0x08000000, LENGTH = 1M
  program (rwx) : ORIGIN = 0x23F00000, LENGTH = 1M
}

SECTIONS
{
    .text :
    {
        . = ALIGN(4);
        *(.text.boot)
        *(.text*)
        . = ALIGN(4);
    } >program

    .data :
    {
        . = ALIGN(4);
        *(.data*);
        *(.rodata*);
    } >program

    _itcm_lma = _end;
    .itcm :
    AT ( _itcm_lma )
    {
        _itcm_s = .;
        KEEP(*(.itcm.start));
        KEEP(*(.itcm*));
        _itcm_e = .;
    } >itcm

    _dtcm_lma = _itcm_lma + (_itcm_e - _itcm_s);
    .dtcm :
    AT ( _dtcm_lma )
    {
        _dtcm_s = .;
        KEEP(*(.dtcm*));
        _dtcm_e = .;
    } >dtcm

    _itcm_len = _itcm_e - _itcm_s;
    _dtcm_len = _dtcm_e - _dtcm_s;

    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _sbss = . ;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = . ;
    } >program

    . = ALIGN(4);
    _end = .;
    __end__ = .;
}
