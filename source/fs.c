#include "common.h"

bool file_exists(const char *path) {
    FIL file;

    if (f_open(&file, path, FA_READ) == FR_OK) {
        f_close(&file);
        return 1;
    }

    else return 0;
}

size_t file_size(const char *path){
    FIL file;
    size_t size = 0;

    if(f_open(&file, path, FA_READ) == FR_OK) {
        size = f_size(&file);
        f_close(&file);
    }

    return size;
}
