#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <ivf/ivf.h>

bool ivf_is_vp8(const ivf_header *hdr)
{
	if ((hdr->magic == IVF_MAGIC) && (hdr->codec_fourcc == VP8_FOURCC)) return true;
    else return false;
}
