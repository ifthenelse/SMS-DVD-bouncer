#include <stdint.h>
#include "SMSlib.h"

// Color palette for rectangle cycling
const uint8_t colors[] = {
    RGB(3, 3, 3), // White
    RGB(3, 0, 0), // Red
    RGB(0, 3, 0), // Green
    RGB(0, 0, 3), // Blue
    RGB(3, 3, 0), // Yellow
    RGB(3, 0, 3), // Magenta
    RGB(0, 3, 3)  // Cyan
};

void main(void)
{
    // Initialize VRAM
    SMS_VRAMmemsetW(0, 0x0000, 16384);

    // Set up sprite mode instead of text mode
    SMS_displayOff();

    // Create a solid 8x8 sprite tile (white block)
    const uint8_t sprite_tile[] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    // Load sprite tile into VRAM at index 0
    SMS_loadTiles(sprite_tile, 0, sizeof(sprite_tile));

    // Set up sprite palette
    SMS_setSpritePaletteColor(0, RGB(0, 0, 0)); // Transparent
    SMS_setSpritePaletteColor(1, colors[0]);    // Rectangle color (start with white)

    // Set background color
    SMS_setBackdropColor(RGB(0, 0, 0));

    // Turn display back on
    SMS_displayOn();

// Rectangle properties (4x2 sprites = 32x16 pixels)
#define RECT_W 4
#define RECT_H 2

    // Bouncing rectangle variables (now in pixel coordinates)
    uint16_t rect_x = 112; // Starting X position (center screen ~256/2)
    uint16_t rect_y = 80;  // Starting Y position (center screen ~192/2)
    int8_t vel_x = 1;      // X velocity
    int8_t vel_y = 1;      // Y velocity
    uint8_t frame_count = 0;
    uint8_t color_idx = 0;
    uint8_t flash_timer = 0;

    while (1)
    {
        SMS_waitForVBlank();

        // Update every 3 frames for smoother movement
        frame_count++;
        if (frame_count >= 3)
        {
            frame_count = 0;

            // Update position
            rect_x += vel_x;
            rect_y += vel_y;

            // Edge collision detection (screen is 256x192, rectangle is 32x16)
            uint8_t hit_edge = 0;

            if (rect_x <= 0)
            {
                rect_x = 0;
                vel_x = -vel_x;
                hit_edge = 1;
            }
            else if (rect_x >= 256 - 32)
            {
                rect_x = 256 - 32;
                vel_x = -vel_x;
                hit_edge = 1;
            }

            if (rect_y <= 0)
            {
                rect_y = 0;
                vel_y = -vel_y;
                hit_edge = 1;
            }
            else if (rect_y >= 192 - 16)
            {
                rect_y = 192 - 16;
                vel_y = -vel_y;
                hit_edge = 1;
            }

            // Change color on edge hit
            if (hit_edge)
            {
                color_idx = (color_idx + 1) % 7;
                SMS_setSpritePaletteColor(1, colors[color_idx]);
            }

            // Corner detection - flash yellow background
            if ((rect_x == 0 || rect_x == 256 - 32) &&
                (rect_y == 0 || rect_y == 192 - 16))
            {
                flash_timer = 25;
            }

            // Handle flash effect
            if (flash_timer > 0)
            {
                SMS_setBackdropColor(RGB(3, 3, 0)); // Yellow flash
                flash_timer--;
            }
            else
            {
                SMS_setBackdropColor(RGB(0, 0, 0)); // Black background
            }

            // Clear all sprites and redraw rectangle
            SMS_initSprites();

            // Draw 4x2 sprite rectangle (32x16 pixels)
            for (uint8_t x = 0; x < RECT_W; x++)
            {
                for (uint8_t y = 0; y < RECT_H; y++)
                {
                    SMS_addSprite(rect_x + (x * 8), rect_y + (y * 8), 0);
                }
            }

            SMS_finalizeSprites();
        }
    }
}

SMS_EMBED_SEGA_ROM_HEADER(9999, 0);
