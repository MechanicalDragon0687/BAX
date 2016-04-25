#include "fs.h"
#include "fatfs/ff.h"

u32 file_exists(const char *path) {
	FIL file;

	if (f_open(&file, path, FA_READ) == FR_OK) {		// Check if file can be opened
		f_close(&file);
		return 1;
	}

	else return 0;
}

u32 file_size(const char *path){
    FIL fp;
    u32 size = 0;

    if(f_open(&fp, path, FA_READ) == FR_OK) {
        size = f_size(&fp);
		f_close(&fp);
	}

    return size;
}
