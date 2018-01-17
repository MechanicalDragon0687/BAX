#pragma once

#include <common.h>

#define FRB_MAX (2048)

typedef struct {
    u32 *data[FRB_MAX]; // Pointers to framebuffers

    u32 store;  // Store position
    u32 fetch;  // Fetch position
    u32 count;  // Frame count

    u32 offset; // Frame offset (in bytes)
    u32 size;   // Frame size (in bytes)
} frb_t;

// Return the frame count
static inline u32 frb_count(frb_t *frb)
{
    return frb->count;
}

// Return the frame offset
static inline u32 frb_offset(frb_t *frb)
{
    return frb->offset;
}

// Return the frame size
static inline u32 frb_size(frb_t *frb)
{
    return frb->size;
}

// Initialize the FRB
void frb_init(frb_t *frb, u32 offset, u32 size);

// Add a frame to the FRB - returns false if full
bool frb_store(frb_t *frb, u32 *data);

// Fetch a frame from the FRB - returns NULL if empty
u32 *frb_fetch(frb_t *frb);

// Drain the FRB, calls `free` on each item if necessary
void frb_drain(frb_t *frb, bool free_frame);
