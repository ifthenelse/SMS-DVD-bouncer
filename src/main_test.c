#include <stdint.h>
#include "SMSlib.h"

void main(void)
{
    SMS_VRAMmemsetW(0, 0x0000, 16384);
    SMS_autoSetUpTextRenderer();
    SMS_printatXY(10, 10, "SIMPLE TEST");
    SMS_setBackdropColor(RGB(3, 0, 0));
    
    while (1) {
        SMS_waitForVBlank();
    }
}

SMS_EMBED_SEGA_ROM_HEADER(9999, 0);
