#include "common.h"
#include "loader.h"

// TODO: Optimize memsearch, possibly search every 2/4 bytes instead of 1
u8 *memsearch(u8* search_start, const u32 search_len, const u8* search_pattern, const u32 pattern_len)
{
    if (!search_start || !search_len || !search_pattern || !pattern_len || (search_len < pattern_len))
        return NULL;

    for (u32 i = 0; i <= (search_len - pattern_len); i++)
    {
        if (memcmp(search_start + i, search_pattern, pattern_len) == 0)
            return (search_start + i);
    }

    return NULL;
}

s32 patch_luma(u8 *buf, u32 len)
{
    const u8 luma_signature[] = {0x01, 0x00, 0x00, 0xEA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // b (pc + 0xC) + dummy int32_t
             sdmc_path_pat[]  = {0x73, 0x00, 0x64, 0x00, 0x6D, 0x00, 0x63, 0x00, 0x3A, 0x00}; // "sdmc:"

    if (memcmp(luma_signature, buf, sizeof(luma_signature))) // Not a Luma3DS/Mizuki/Salt payload
        return 0;

    const char payload_path[] = PAYLOAD_PATH;

    u8 *sdmc_loc = memsearch(buf, len, sdmc_path_pat, sizeof(sdmc_path_pat));

    if (!sdmc_loc) // Reboot patch path not found
        return -1; // Yes, this is bad

    sdmc_loc += sizeof(sdmc_path_pat); // Skip "sdmc:"

    u32 path_len = strlen(payload_path);

    // Zero out the original path
    memset(sdmc_loc, 0, 74);

    // Copy the current path to memory
    for (u32 i = 0; i < path_len; i++)
        sdmc_loc[i*2] = payload_path[i];

    return 0;
}

void chainload()
{
    // loader itself gets loaded @ 0x25F00000, the payload gets loaded @ 0x24F00000
    u8 *loader_addr = (u8*)0x25F00000, *payload_addr = (u8*)0x24F00000;

    FIL payload;
    size_t payload_br;

    memcpy(loader_addr, loader_bin, loader_bin_len);

    if (f_open(&payload, PAYLOAD_PATH, FA_READ) != FR_OK)
        error("Missing " PAYLOAD_PATH);

    // Read to memory where the loader expects
    f_read(&payload, payload_addr, f_size(&payload), &payload_br);

    f_close(&payload);

    // Patch payloads that use Luma-styled reboot patches (Luma itself, Mizuki, Salt, etc)
    if (patch_luma(payload_addr, payload_br) != 0)
        error("Luma patching failed!");
        // This can only happen if the payload is detected as a Luma payload but no patterns could be found

    f_mount(NULL, "0:", 1); // Unmount SD

    // Jump to the loader
    ((void(*)())(u32)loader_addr)();
}

inline void set_pixel(const u8 *fb, const u16 x, const u16 y, const u32 rgb)
{
	u32 offset = (240 * (x) + 240 - (y) - 1) * 3;
	*(u8*)(fb + offset++) = rgb & 0xFF;
	*(u8*)(fb + offset++) = (rgb >> 8) & 0xFF;
	*(u8*)(fb + offset++) = (rgb >> 16) & 0xFF;
}

void error(const char *msg)
{
    u8 *errtxt_fb = TOP_SCREEN0, *lsd_fb = BOT_SCREEN0;

    draw_str(errtxt_fb, msg, 8, 8, 0xFFFFFF);
    draw_str(errtxt_fb, "Press any key to power off", 8, 24, 0xFFFFFF);

    // Fun stuff
    u8 i = 0;
    u16 x, y;

    while(1)
    {
        i = i % 0x17;

        for (y = 0; y < 240; y++)
        {
            for (x = 0; x < (lsd_fb == TOP_SCREEN0 ? 400 : 320); x++)
            {
                set_pixel(BOT_SCREEN0, x, y, (x << i) ^ (y << i));
            }
        }

        i++;
        if (HID_PAD)
            poweroff();
    }
}
