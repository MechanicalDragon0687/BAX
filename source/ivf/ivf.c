#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <ivf/ivf.h>

bool check_ivf(const ivf_header *hdr)
{
	if ((hdr->magic == IVF_MAGIC) && (hdr->codec_fourcc == VP8_FOURCC)) return true;
    else return false;
}
