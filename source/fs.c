#include "common.h"

bool file_exists(const char *path) {
    FIL file;

    if (f_open(&file, path, FA_READ) == FR_OK) { // If file was successfully opened
        f_close(&file); // Close it and return 1
        return 1;
    }

    else return 0;
}

size_t file_size(const char *path){
    FIL file;
    size_t size = 0;

    if(f_open(&file, path, FA_READ) == FR_OK) { // If file was opened successfully
        size = f_size(&file); // Get size, close and return size
        f_close(&file);
    }

    return size;
}
