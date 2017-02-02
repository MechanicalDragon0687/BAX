#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// https://wiki.multimedia.cx/index.php?title=IVF

#define IVF_MAGIC  ('D' | 'K' << 8 | 'I' << 16 | 'F' << 24) // 'DKIF'
#define VP8_FOURCC ('V' | 'P' << 8 | '8' << 16 | '0' << 24) // 'VP80'

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
	uint32_t dummy;
} ivf_header;

bool check_ivf(const ivf_header *hdr);
