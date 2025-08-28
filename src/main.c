#include <stdint.h>
#include "SMSlib.h"

// Rectangle properties
#define RECT_WIDTH 32
#define RECT_HEIGHT 16
#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 192

// Rectangle state
int16_t rect_x = 50;     // Starting X position
int16_t rect_y = 50;     // Starting Y position
int8_t velocity_x = 2;   // X velocity (45 degree movement)
int8_t velocity_y = 2;   // Y velocity (45 degree movement)
uint8_t rect_color = 0;  // Current rectangle color index
uint8_t flash_timer = 0; // Timer for yellow flash effect

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
    // Initialize the system - this approach will definitely work
    SMS_VRAMmemsetW(0, 0x0000, 16384);
    SMS_autoSetUpTextRenderer(); // This we know works from our test

    // Start with something we know is visible
    SMS_printatXY(8, 10, "BOUNCER TEST");
    SMS_printatXY(8, 12, "COLORS:");

    // Simple animation loop - change background color to show it's running
    uint8_t color_cycle = 0;
    uint8_t frame_counter = 0; // Move this outside static
    const uint8_t bg_colors[] = {
        RGB(3, 0, 0), // Red
        RGB(0, 3, 0), // Green
        RGB(0, 0, 3), // Blue
        RGB(3, 3, 0)  // Yellow
    };
    const char *color_names[] = {"RED  ", "GREEN", "BLUE ", "YELLOW"};

    while (1)
    {
        SMS_waitForVBlank();

        // Every 30 frames (0.5 second), change background color
        frame_counter++;
        if (frame_counter >= 30)
        {
            frame_counter = 0;
            color_cycle = (color_cycle + 1) % 4;

            // Set backdrop color
            SMS_setBackdropColor(bg_colors[color_cycle]);

            // Also change text background palette to make it more visible
            SMS_setBGPaletteColor(0, bg_colors[color_cycle]);

            // Update the color name text
            SMS_printatXY(15, 12, color_names[color_cycle]);
        }
    }
}

SMS_EMBED_SEGA_ROM_HEADER(9999, 0);