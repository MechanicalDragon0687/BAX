# BAX Memory Map

## ARM9:

| Name     | Start      | Size  | IP    | DP    | IDB   | Usage                      | Notes |
| :------- | :--------: | :---: | :---: | :---: | :---: | :------------------------- | :---- |
| ITCM     | 0x00000000 | 128MB | RO_NA | RW_NA |  nnn  | Code                       | None |
| ARM9 RAM | 0x08000000 |  1MB  | RO_NA | RW_NA |  yyy  | Boot code, data/BSS/stacks | None |
| MMIO     | 0x10000000 |  2MB  | NA_NA | RW_NA |  nnn  | Device IO                  | None |
| VRAM     | 0x18000000 |  8MB  | NA_NA | RW_NA |  nnn  | VRAM                       | Physically 6MB |
| AXIRAM   | 0x1FF00000 |  1MB  | NA_NA | RW_NA |  nnn  | MPCore executable/data     | DO. NOT. TOUCH. |
| FCRAM    | 0x20000000 | 128MB | NA_NA | RW_NA |  nnn  | None                       | None |
| DTCM     | 0x30000000 |  16KB | NA_NA | RW_NA |  nnn  | None                       | None |
| BootROM  | 0xFFFF0000 |  32KB | RO_NA | RO_NA |  ynn  | High exception vectors     | None |



## ARM11:

| Name    | PA         | VA         | Size  | Attribute                 | AP    | XN    | Usage                   | Notes |
| :------ | :--------: | :--------: | :---: | :------------------------ | :---: | :---: | :---------------------- | :---: |
| MMIO    | 0x10100000 | 0x10100000 | 4MB   | Non-shared device         | RW_NA |   y   | IO                      | None  |
| PMR     | 0x17E00000 | 0x17E00000 | 1MB   | Non-shared device         | RW_NA |   n   | Internal CPU registers  | None  |
| VRAM    | 0x18000000 | 0x18000000 | 6MB   | Writethrough, no wr alloc | RW_NA |   y   | Framebuffers            | None  |
| AXIRAM  | 0x1FF00000 | 0x1FF00000 | 1MB   | Writeback, wr alloc       | RW_NA |   n   | Code, data, BSS, stacks | None  |
| FCRAM   | 0x20000000 | 0x20000000 | 128MB | Writeback, wr alloc       | RW_NA |   y   | Heap                    | None  |
| BootROM | 0xFFF00000 | 0xFFF00000 | 1MB   | Non-cacheable             | RO_NA |   n   | Exception router        | None  |
