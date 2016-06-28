#include "common.h"

void chainload()
{
    // loader itself gets loaded @ 0x25F00000, the payload gets loaded @ 0x24F00000
    u8 *loader_addr = (u8*)0x25F00000, *payload_addr = (u8*)0x24F00000;

    FIL loader, payload;
    size_t loader_br, payload_br;

    // Attempt to load both the loader and the payload
    if (f_open(&loader, LOADER_PATH, FA_READ) != FR_OK)
        error("Couldn't load " LOADER_PATH);

    if (f_open(&payload, PAYLOAD_PATH, FA_READ) != FR_OK)
        error("Couldn't load " PAYLOAD_PATH);

    // Read to memory where the loader expects
    f_read(&loader, loader_addr, f_size(&loader), &loader_br);
    f_read(&payload, payload_addr, f_size(&payload), &payload_br);

    f_close(&loader);
    f_close(&payload);

    // Jump to the loader
    ((void(*)())loader_addr)();
}
