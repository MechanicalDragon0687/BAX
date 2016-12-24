#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <disk/fatfs/ff.h>
#include <disk/sdmmc/sdmmc.h>

bool file_exists(const char *path)
{
    FIL f;
    int ret = f_open(&f, path, FA_READ);
    if (!ret) f_close(&f);
    return (!ret);
}

size_t file_size(const char *path)
{
    FIL f;
    size_t ret = 0;
    int fz = f_open(&f, path, FA_READ);
    if (!fz)
    {
        ret = f_size(&f);
        f_close(&f);
    }
    return ret;
}

size_t file_read_offset(uint8_t *dest, const char *path, const size_t offset, const size_t max_size)
{
    FIL f;
    size_t ret = 0;
    int fz = f_open(&f, path, FA_READ);
    if (!fz)
    {
        f_lseek(&f, offset);
        fz = f_read(&f, dest, max_size, &ret);
        f_close(&f);
    }
    return ret;
}

size_t file_read(uint8_t *dest, const char *path, const size_t max_size)
{
    return file_read_offset(dest, path, 0, max_size);
}

uint32_t find_files(const char *base_path, const char *pattern, uint32_t max, char out[][_MAX_LFN + 1])
{
    uint32_t ret, ctr = 0;
    DIR dirent;
    FILINFO fno;
    ret = f_findfirst(&dirent, &fno, base_path, pattern);
    size_t base_len = strlen(base_path);
    while (ret == FR_OK && fno.fname[0] && ctr < max)
    {
        if (!(fno.fattrib & AM_DIR) && out[ctr])
        {
            strcpy(out[ctr], base_path);
            out[ctr][base_len] = '/';
            strcat(out[ctr], fno.fname);
            ctr++;
        }
        ret = f_findnext(&dirent, &fno);        
    }
    f_closedir(&dirent);
    return ctr;
}
