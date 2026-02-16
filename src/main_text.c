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
    SMS_VRAMmemsetW(0, 0x0000, 16384);
    SMS_autoSetUpTextRenderer();
    SMS_printatXY(6, 1, "SMS DVD BOUNCER");
    SMS_setBackdropColor(RGB(0, 0, 0)); // Black background

// Rectangle properties (4x2 characters = 32x16 pixels)
#define RECT_W 4
#define RECT_H 2

    // Bouncing rectangle variables
    uint8_t rect_x = 14; // Starting position
    uint8_t rect_y = 10;
    uint8_t old_x = rect_x;
    uint8_t old_y = rect_y;
    int8_t vel_x = 1; // 45-degree movement
    int8_t vel_y = 1;
    uint8_t frame_count = 0;
    uint8_t color_idx = 0;
    uint8_t flash_timer = 0;

    // Set initial rectangle color
    SMS_setBGPaletteColor(1, colors[color_idx]);

    while (1)
    {
        SMS_waitForVBlank();

        // Update every 6 frames for smooth movement
        frame_count++;
        if (frame_count >= 6)
        {
            frame_count = 0;

            // Clear old rectangle
            for (uint8_t x = 0; x < RECT_W; x++)
            {
                for (uint8_t y = 0; y < RECT_H; y++)
                {
                    SMS_printatXY(old_x + x, old_y + y, " ");
                }
            }

            // Update position
            old_x = rect_x;
            old_y = rect_y;
            rect_x += vel_x;
            rect_y += vel_y;

            // Edge collision detection with color change
            uint8_t hit_edge = 0;

            if (rect_x <= 0)
            {
                rect_x = 0;
                vel_x = -vel_x;
                hit_edge = 1;
            }
            else if (rect_x >= 32 - RECT_W)
            {
                rect_x = 32 - RECT_W;
                vel_x = -vel_x;
                hit_edge = 1;
            }

            if (rect_y <= 3)
            { // Avoid title
                rect_y = 3;
                vel_y = -vel_y;
                hit_edge = 1;
            }
            else if (rect_y >= 24 - RECT_H)
            {
                rect_y = 24 - RECT_H;
                vel_y = -vel_y;
                hit_edge = 1;
            }

            // Change color on edge hit
            if (hit_edge)
            {
                color_idx = (color_idx + 1) % 7;
                SMS_setBGPaletteColor(1, colors[color_idx]);
            }

            // Corner detection - flash yellow background
            if ((rect_x == 0 || rect_x == 32 - RECT_W) &&
                (rect_y == 3 || rect_y == 24 - RECT_H))
            {
                flash_timer = 25; // Flash for ~0.4 seconds
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

            // Draw rectangle (4x2 block of solid characters)
            for (uint8_t x = 0; x < RECT_W; x++)
            {
                for (uint8_t y = 0; y < RECT_H; y++)
                {
                    SMS_printatXY(rect_x + x, rect_y + y, "#");
                }
            }
        }
    }
}

SMS_EMBED_SEGA_ROM_HEADER(9999, 0);
