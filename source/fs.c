#include "fs.h"
#include "fatfs/ff.h"

u32 fileExists(char *path) {
	if (f_stat(path, NULL) == FR_OK) // Check if file can be opened
		return 1;

	else return 0;
}

u32 fileSize(const char *path){
    FIL fp;
    u32 size = 0;

    if(f_open(&fp, path, FA_READ) == FR_OK) // Open the file
        size = f_size(&fp); // Get on the floor

    f_close(&fp); // Everybody walk the dinosaur
    return size;
}
