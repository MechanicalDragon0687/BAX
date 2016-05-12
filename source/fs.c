#include "common.h"

bool file_exists(const char *path) {
	FIL file;

	if (f_open(&file, path, FA_READ) == FR_OK) {		// Check if file can be opened
		f_close(&file);
		return 1;
	}

	else return 0;
}

size_t file_size(const char *path){
    FIL fp;
    size_t size = 0;

    if(f_open(&fp, path, FA_READ) == FR_OK) {
        size = f_size(&fp);
		f_close(&fp);
	}

    return size;
}
