OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(__boot)

MEMORY
{
  itcm (rwx)    : ORIGIN = 0x00000000, LENGTH = 32K
  dtcm (rw)     : ORIGIN = 0xFFF00000, LENGTH = 16K
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

    _itcm_len = _itcm_e - _itcm_s;
    _dtcm_len = _dtcm_e - _dtcm_s;

    _itcm_loc = ORIGIN(itcm);
    _dtcm_loc = ORIGIN(dtcm);

    ASSERT (_itcm_len <= LENGTH(itcm), "ITCM binary is too large")
    ASSERT (_dtcm_len <= LENGTH(dtcm), "DTCM binary is too large")

    _abt_stack = 0x23F00000;
    _irq_stack = 0x00008000;
    _prg_stack = 0x08100000;

    _end = .;
    __end__ = .;
}
