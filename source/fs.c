#include "fs.h"
#include "fatfs/ff.h"

static FATFS fs;

u32 mountSDMC(void){
    if(f_mount(&fs, "0:", 1) != FR_OK) return 0;
    return 1;
}

u32 fileSize(const char *path){
    FIL fp;
    u32 size = 0;

    if(f_open(&fp, path, FA_READ) == FR_OK)
        size = f_size(&fp);

    f_close(&fp);
    return size;
}
