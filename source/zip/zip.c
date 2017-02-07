#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <common.h>
#include <zip/zip.h>

void *memmem(const void *haystack, size_t haystacklen, const void *needle, size_t needlelen);

// TODO: Add CRC checks, etc
// TODO: Do it THE RIGHT WAY™
size_t zip_extract_file(const char *filename, void *dest, size_t max_size, u8 *zip_data, size_t zip_len)
{
    u8 *dataptr = zip_data;
    zipf_entry *zipf;

    u32 magic = ZIP_MAGIC;
    u32 filename_len = strlen(filename);

    while(1)
    {
        zipf = (zipf_entry*)memmem(dataptr, (zip_data + zip_len - dataptr), (u8*)&magic, 4);
        if (!zipf) break;
        if ((zipf->comp == 0) && (zipf->raw_size == zipf->size))
        { // ZIP entry uses Store and sizes check out
            char *zipf_name = (char*)(zipf + sizeof(zipf_entry));
            if (!memcmp(zipf_name, filename, filename_len))
            { // Found the correct file, copy to dest
                u32 cpy_size = (zipf->size > max_size) ? max_size : zipf->size;
                void *src = (zipf + sizeof(zipf_entry) + zipf->name_len + zipf->meta_len);
                // End of entry, beginning of data

                memcpy(dest, src, cpy_size);
                return cpy_size;
            }
        }
        dataptr = (u8*)zipf;
    }

    return 0;
}
