#include <stdint.h>
#include "SMSlib.h"

void main(void)
{
    /* Clear VRAM first */
    SMS_VRAMmemsetW(0, 0x0000, 16384);

    /* Set up text renderer (loads font, sets up palettes, turns on display) */
    SMS_autoSetUpTextRenderer();

    /* Print test message */
    SMS_printatXY(8, 10, "SMS DVD BOUNCER");
    SMS_printatXY(10, 12, "WORKING!");

    /* Set backdrop color to bright red (should show around the edges) */
    SMS_setBackdropColor(RGB(3, 0, 0));

    /* Infinite loop */
    for (;;)
    {
    }
}

SMS_EMBED_SEGA_ROM_HEADER(9999, 0);