#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// https://wiki.multimedia.cx/index.php?title=IVF

#define IVF_MAGIC  ('F' << 24 | 'I' << 16 | 'K' << 8 | 'D') // 'DKIF'
#define VP8_FOURCC ('0' << 24 | '8' << 16 | 'P' << 8 | 'V') // 'VP80'

typedef struct
{
	uint32_t magic;
	uint16_t version;
	uint16_t hdr_len;
	uint32_t codec_fourcc;
	uint16_t width;
	uint16_t height;
	uint32_t framerate;
	uint32_t timescale;
	uint32_t framecount;
	uint32_t unused;
} ivf_header;

bool ivf_is_vp8(const ivf_header *hdr);
