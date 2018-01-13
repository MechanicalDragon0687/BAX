#include <common.h>
#include "frb.h"

void frb_init(frb_t *frb, u32 offset, u32 size) {
	memset(frb, 0, sizeof(frb_t));
	frb->offset = offset;
	frb->size = size;
	return;
}

bool frb_store(frb_t *frb, u32 *data)
{
	bool ret = false;
	if (frb_count(frb) < FRB_MAX)
	{
		frb->data[frb->store] = data;
		frb->store = (frb->store + 1) % FRB_MAX;
		frb->count++;
		ret = true;
	}
	return ret;
}

u32 *frb_fetch(frb_t *frb)
{
	u32 *ret = NULL;
	if (frb_count(frb) > 0)
	{
		ret = frb->data[frb->fetch];
		frb->fetch = (frb->fetch + 1) % FRB_MAX;
		frb->count--;
	}
	return ret;
}

void frb_drain(frb_t *frb, bool free_frame)
{
	u32 *frame;
	while(frb_count(frb) > 0)
	{
		frame = frb_fetch(frb);
		if (free_frame) free(frame);
	}
	return;
}
