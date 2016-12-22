#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <zip/zip.h>

#include <io/printf.h>

// TODO: Add ZIP error handling
// TODO2: Do it THE RIGHT WAY™
size_t zip_extract_file(const char *filename, uint8_t *dest, uint8_t *zip_data, const size_t max_size)
{
    uint8_t *dataptr = zip_data;
    zipf_entry *zipf = (zipf_entry*)dataptr;

    while((zipf->magic == ZIP_MAGIC) && (zipf->comp == 0) && (zipf->raw_size == zipf->size))
    {
        size_t fname_len = strlen(filename);
        if (!memcmp(filename, (char*)(dataptr + sizeof(zipf_entry)), fname_len))
        {
            size_t extsize = (zipf->size > max_size) ? max_size : zipf->size;
            memcpy(dest, (char*)(dataptr + sizeof(zipf_entry) + zipf->name_len + zipf->meta_len), extsize);
            return extsize;
        }
        dataptr += sizeof(zipf_entry) + zipf->name_len + zipf->meta_len + zipf->size;
        zipf = (zipf_entry*)dataptr;
    }
    return 0;
}
