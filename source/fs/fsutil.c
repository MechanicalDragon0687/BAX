#include <common.h>
#include <fs/ff/ff.h>

static char _rnd_file_path[_MAX_LFN + 1];

char *get_random_file(char *dir, char *pattern, int max)
{
    DIR dp;
    FILINFO dinfo;
    FRESULT res;
    int count, rnd;
    char paths[max][_MAX_LFN + 1];

    memset(_rnd_file_path, 0, _MAX_LFN + 1);
    count = 0;
    res = f_findfirst(&dp, &dinfo, dir, pattern);
    while ((res == FR_OK) && (*(dinfo.fname)) && (count < max)) {
        if (!(dinfo.fattrib & AM_DIR)) {
            strcpy(paths[count++], dinfo.fname);
        }
        res = f_findnext(&dp, &dinfo);
    }
    f_closedir(&dp);

    if (count != 0) {
        rnd = read_rand() % count;
        strcpy(_rnd_file_path, dir);
        strcpy(_rnd_file_path + strlen(_rnd_file_path), "/");
        strcpy(_rnd_file_path + strlen(_rnd_file_path), paths[rnd]);        
    }

    return _rnd_file_path;
}
