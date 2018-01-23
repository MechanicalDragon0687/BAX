#include <common.h>

#include "lib/ff/ff.h"

static FATFS fs;
int fs_init(void)
{
    return f_mount(&fs, "sdmc:", 1);
}

int fs_list(const char *dirpath, const char *pattern, char **paths, int max)
{
    int ret;
    FRESULT res;
    DIR dir;
    FILINFO filinf;

    for (int i = 0; i < max; i++) paths[i] = NULL;
    ret = 0;

    res = f_findfirst(&dir, &filinf, dirpath, pattern);
    while((res == FR_OK) && (filinf.fname[0] != 0) && (ret < max))
    {
        if (!(filinf.fattrib & AM_DIR))
        {
            paths[ret] = malloc(strlen(filinf.fname) + 1);
            strcpy(paths[ret], filinf.fname);
            ret++;
        }
        res = f_findnext(&dir, &filinf);
    }
    return ret;
}

size_t fs_read(const char *path, void *dest, size_t bytes)
{
    size_t ret;
    FIL f;

    if (f_open(&f, path, FA_READ) == FR_OK)
    {
        f_read(&f, dest, bytes, &ret);
        f_close(&f);
    }
    else
    {
        ret = 0;
    }

    return ret;
}

size_t fs_size(const char *path)
{
    size_t ret;
    FIL f;

    if (f_open(&f, path, FA_READ) == FR_OK)
    {
        ret = f_size(&f);
        f_close(&f);
    }
    else
    {
        ret = 0;
    }
    return ret;
}
