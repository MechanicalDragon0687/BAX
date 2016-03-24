#include "fs.h"
#include "fatfs/ff.h"
#include "types.h"

static FATFS fs;

u32 mountSDMC(void){
    if (f_mount(&fs, "0:", 1) == FR_OK) return 1;
	return 0;
}

u32 fileReadOffset(u8 *dest, const char *path, u32 offset, u32 size) {
    FRESULT fr;
    FIL fp;
    unsigned int br = 0;

    fr = f_open(&fp, path, FA_READ);
    if(fr == FR_OK){
        if(!size) size = f_size(&fp);
		f_lseek(&fp, offset);
        fr = f_read(&fp, dest, size, &br);
    }

    f_close(&fp);
    return fr ? 0 : 1;
}

u32 fileRead(u8 *dest, const char *path, u32 size){
    FRESULT fr;
    FIL fp;
    unsigned int br = 0;

    fr = f_open(&fp, path, FA_READ);
    if(fr == FR_OK){
        if(!size) size = f_size(&fp);
        fr = f_read(&fp, dest, size, &br);
    }

    f_close(&fp);
    return fr ? 0 : 1;
}

u32 fileSize(const char *path){
    FIL fp;
    u32 size = 0;

    if(f_open(&fp, path, FA_READ) == FR_OK)
        size = f_size(&fp);

    f_close(&fp);
    return size;
}

u32 fileExists(const char *path){
    FIL fp;
    u32 exists = 0;

    if(f_open(&fp, path, FA_READ) == FR_OK) exists = 1;

    f_close(&fp);
    return exists;
}