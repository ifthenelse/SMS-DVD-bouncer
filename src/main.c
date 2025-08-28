#include <stdint.h>
#include <stdbool.h>
#include "SMSlib.h"

// Compatibility shim: some SMSlib versions provide `SMS_doVBlankProcess()` to
// flush queued VRAM/CRAM/sprite updates from the VBlank handler. Older
// devkitSMS releases may not export this function. Provide a minimal
// no-op implementation here so the code links on either version.
//
// If your SMSlib already flushes queued updates in the ISR, this stub is
// harmless. If you have a newer SMSlib that provides a proper
// `SMS_doVBlankProcess`, remove this stub.
void SMS_doVBlankProcess(void) { /* compatibility no-op */ }

SMS_EMBED_SEGA_ROM_HEADER(0, 0);

// one solid 8×8 tile (all pixels = color index 1)
static const unsigned char TILE_SOLID_COLOR1[32] = {
    0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
    0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
    0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
    0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00};

// PSG (simple beep on corner)
__sfr __at(0x7F) PSG;
static inline void PSGWrite(uint8_t v) { PSG = v; }
static inline void tone_ch0(uint16_t p)
{
    PSGWrite(0x80 | (p & 0x0F));
    PSGWrite((p >> 4) & 0x3F);
}
static inline void vol_ch0(uint8_t a) { PSGWrite(0x90 | (a & 0x0F)); } // 0 loud..15 mute

#define COLS 32
#define ROWS 24
#define RECT_TW 4   // 32 px
#define RECT_TH 2   // 16 px
#define TILE_RECT 1 // use tile index 1; tile 0 stays blank

// tiny RNG
static uint16_t rng = 0xA1C3;
static uint16_t rand16(void)
{
    uint16_t b = rng & 1;
    rng >>= 1;
    if (b)
        rng ^= 0xB400;
    return rng;
}
static void set_random_rect_color(void)
{
    uint16_t v = rand16();
    uint8_t r = 1 + ((v) & 3) % 3;
    uint8_t g = 1 + ((v >> 2) & 3) % 3;
    uint8_t b = 1 + ((v >> 4) & 3) % 3;
    SMS_setBGPaletteColor(1, RGB(r, g, b)); // color for our solid tile
}

static void draw_rect_tiles(uint8_t tx, uint8_t ty, uint8_t tile)
{
    for (uint8_t r = 0; r < RECT_TH; r++)
    {
        SMS_setNextTileatXY(tx, ty + r);
        for (uint8_t c = 0; c < RECT_TW; c++)
            SMS_setTile(tile);
    }
}

int main(void)
{
    // IMPORTANT: do NOT disable interrupts; SMSlib needs them for vblank + queue flush
    SMS_init();

    // palettes
    SMS_setBGPaletteColor(0, RGB(0, 0, 3)); // blue backdrop
    SMS_setBGPaletteColor(1, RGB(3, 3, 3)); // white for our rect (will randomize on bounces)
    for (uint8_t i = 2; i < 16; i++)
        SMS_setBGPaletteColor(i, RGB(0, 0, 0));

    // tiles
    SMS_loadTiles(TILE_SOLID_COLOR1, TILE_RECT, 1);

    // clear BG
    SMS_setNextTileatXY(0, 0);
    for (uint16_t i = 0; i < COLS * ROWS; i++)
        SMS_setTile(0);

    SMS_displayOn(); // turn on video

    // position in TILE units
    int16_t tx = (COLS - RECT_TW) / 2, ty = (ROWS - RECT_TH) / 2;
    int8_t dx = 1, dy = 1;
    uint8_t flash = 0, beep = 0;

    set_random_rect_color();
    draw_rect_tiles((uint8_t)tx, (uint8_t)ty, TILE_RECT);

    for (;;)
    {
        // ---- begin frame: flush last frame's queued VRAM/CRAM updates ----
        SMS_waitForVBlank();
        SMS_doVBlankProcess();
        // -----------------------------------------------------------------

        // erase previous (queued for NEXT vblank)
        draw_rect_tiles((uint8_t)tx, (uint8_t)ty, 0);

        // update position
        tx += dx;
        ty += dy;
        uint8_t hitX = 0, hitY = 0;
        if (tx <= 0)
        {
            tx = 0;
            dx = 1;
            hitX = 1;
        }
        else if (tx >= COLS - RECT_TW)
        {
            tx = COLS - RECT_TW;
            dx = -1;
            hitX = 1;
        }
        if (ty <= 0)
        {
            ty = 0;
            dy = 1;
            hitY = 1;
        }
        else if (ty >= ROWS - RECT_TH)
        {
            ty = ROWS - RECT_TH;
            dy = -1;
            hitY = 1;
        }

        if (hitX || hitY)
        {
            set_random_rect_color(); // queued, will apply on next flush
            if (hitX && hitY)
            { // corner
                flash = 8;
                tone_ch0(0x0180);
                vol_ch0(8);
                beep = 10;
            }
        }

        // flash palette 0 (queued)
        if (flash)
        {
            SMS_setBGPaletteColor(0, RGB(3, 3, 0));
            --flash;
        }
        else
        {
            SMS_setBGPaletteColor(0, RGB(0, 0, 3));
        }

        // stop beep after a few frames (not queued; writes straight to PSG)
        if (beep && --beep == 0)
            vol_ch0(15);

        // draw new rect (queued)
        draw_rect_tiles((uint8_t)tx, (uint8_t)ty, TILE_RECT);
        // loop → next VBlank will flush all the queued changes
    }
}