ENTRY(_start)
SECTIONS
{
    . = 0x25F00000;
    .text.start : { *(.text.start) }
    . = ALIGN(4);
}
