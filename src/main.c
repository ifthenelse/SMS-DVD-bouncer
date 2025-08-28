#include <stdint.h>
#include "SMSlib.h"

void main(void)
{
    /* Clear VRAM first - this is crucial */
    SMS_VRAMmemsetW(0, 0x0000, 16384);

    /* Set up text renderer (initializes palettes and display) */
    SMS_autoSetUpTextRenderer();

    /* Set backdrop to bright red */
    SMS_setBackdropColor(RGB(3, 0, 0));

    /* Do nothing - just display red screen */
    for (;;)
    {
    }
}

SMS_EMBED_SEGA_ROM_HEADER(9999, 0);