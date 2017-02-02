.align 4

.global a11bin
a11bin:
    .incbin "ARM11/build/main.bin"
a11bin_end:

.global a11bin_len
a11bin_len:
    .word (a11bin_end-a11bin)
