#include <stdbool.h>
#include <stdint.h>
#include "SMSlib.h"
#include "dvd_logo.h"

#define SCREEN_TILE_W 32
#define SCREEN_TILE_H 24
#define LOGO_TILE_BASE 1
#define LOGO_MAX_X (SCREEN_TILE_W - DVD_LOGO_TILE_W)
#define LOGO_MAX_Y (SCREEN_TILE_H - DVD_LOGO_TILE_H)

#define TILE_EMPTY 0

#define PALETTE_BG 0
#define PALETTE_LOGO 1
#define PALETTE_FLASH 15

#define HEARTBEAT_PERIOD 24
#define MOVE_PERIOD 10
#define FLASH_FRAMES 14
#define BEEP_FRAMES 18

#define PSG_CH0_TONE_LATCH 0x80
#define PSG_CH0_VOL_LATCH 0x90
#define PSG_VOL_SILENT 0x0F

__sfr __at (0x7F) PSGPort;

static const unsigned char rect_colors[] = {
    RGB(3, 3, 3),
    RGB(3, 0, 0),
    RGB(0, 3, 0),
    RGB(0, 0, 3),
    RGB(3, 3, 0),
    RGB(3, 0, 3),
    RGB(0, 3, 3)};

#define RECT_COLOR_COUNT 7

static void draw_logo(unsigned char x, unsigned char y, bool visible) {
    unsigned char tx;
    unsigned char ty;
    unsigned int tile;

    for (ty = 0; ty < DVD_LOGO_TILE_H; ++ty) {
        for (tx = 0; tx < DVD_LOGO_TILE_W; ++tx) {
            tile = visible ? (LOGO_TILE_BASE + (ty * DVD_LOGO_TILE_W) + tx) : TILE_EMPTY;
            SMS_setTileatXY(x + tx, y + ty, tile);
        }
    }
}

static void start_corner_beep(unsigned char *beep_timer) {
    const unsigned int tone_period = 0x120;

    PSGPort = PSG_CH0_TONE_LATCH | (tone_period & 0x0F);
    PSGPort = (tone_period >> 4) & 0x3F;
    PSGPort = PSG_CH0_VOL_LATCH | 0x00;
    *beep_timer = BEEP_FRAMES;
}

static void update_corner_beep(unsigned char *beep_timer) {
    if (*beep_timer == 0) {
        return;
    }

    --(*beep_timer);
    if (*beep_timer == BEEP_FRAMES - 6) {
        PSGPort = PSG_CH0_VOL_LATCH | 0x04;
    } else if (*beep_timer == BEEP_FRAMES - 12) {
        PSGPort = PSG_CH0_VOL_LATCH | 0x08;
    } else if (*beep_timer == 0) {
        PSGPort = PSG_CH0_VOL_LATCH | PSG_VOL_SILENT;
    }
}

void main(void) {
    unsigned char x;
    unsigned char y;
    signed char logo_x = 11;
    signed char logo_y = 10;
    signed char old_x = logo_x;
    signed char old_y = logo_y;
    signed char velocity_x = 1;
    signed char velocity_y = 1;
    unsigned char color_index = 0;
    unsigned char heartbeat_counter = 0;
    unsigned char move_counter = 0;
    bool heartbeat_on = false;
    unsigned char flash_timer = 0;
    unsigned char beep_timer = 0;

    SMS_displayOff();
    SMS_VRAMmemsetW(0, 0x0000, 16384);
    SMS_load1bppTiles(dvd_logo_1bpp, LOGO_TILE_BASE, sizeof(dvd_logo_1bpp), 0, PALETTE_LOGO);

    SMS_setBGPaletteColor(PALETTE_BG, RGB(0, 0, 0));
    SMS_setBGPaletteColor(PALETTE_LOGO, rect_colors[color_index]);
    SMS_setBGPaletteColor(PALETTE_FLASH, RGB(3, 3, 0));
    SMS_setBackdropColor(PALETTE_BG);

    for (y = 0; y < SCREEN_TILE_H; ++y) {
        for (x = 0; x < SCREEN_TILE_W; ++x) {
            SMS_setTileatXY(x, y, TILE_EMPTY);
        }
    }

    draw_logo((unsigned char)logo_x, (unsigned char)logo_y, true);
    PSGPort = PSG_CH0_VOL_LATCH | PSG_VOL_SILENT;
    SMS_displayOn();

    for (;;) {
        bool hit_x = false;
        bool hit_y = false;

        SMS_waitForVBlank();

        ++heartbeat_counter;
        if (heartbeat_counter >= HEARTBEAT_PERIOD) {
            heartbeat_counter = 0;
            heartbeat_on = !heartbeat_on;
            if (flash_timer == 0) {
                SMS_setBGPaletteColor(PALETTE_BG, heartbeat_on ? RGB(0, 0, 1) : RGB(0, 0, 0));
            }
        }

        if (flash_timer > 0) {
            SMS_setBackdropColor(PALETTE_FLASH);
            --flash_timer;
            if (flash_timer == 0) {
                SMS_setBackdropColor(PALETTE_BG);
            }
        }

        update_corner_beep(&beep_timer);

        ++move_counter;
        if (move_counter < MOVE_PERIOD) {
            continue;
        }
        move_counter = 0;

        draw_logo((unsigned char)old_x, (unsigned char)old_y, false);

        logo_x += velocity_x;
        logo_y += velocity_y;

        if (logo_x < 0) {
            logo_x = 0;
            velocity_x = 1;
            hit_x = true;
        } else if (logo_x > LOGO_MAX_X) {
            logo_x = LOGO_MAX_X;
            velocity_x = -1;
            hit_x = true;
        }

        if (logo_y < 0) {
            logo_y = 0;
            velocity_y = 1;
            hit_y = true;
        } else if (logo_y > LOGO_MAX_Y) {
            logo_y = LOGO_MAX_Y;
            velocity_y = -1;
            hit_y = true;
        }

        if (hit_x || hit_y) {
            ++color_index;
            if (color_index >= RECT_COLOR_COUNT) {
                color_index = 0;
            }
            SMS_setBGPaletteColor(PALETTE_LOGO, rect_colors[color_index]);
        }

        if (hit_x && hit_y) {
            flash_timer = FLASH_FRAMES;
            start_corner_beep(&beep_timer);
        }

        draw_logo((unsigned char)logo_x, (unsigned char)logo_y, true);
        old_x = logo_x;
        old_y = logo_y;
    }
}

SMS_EMBED_SEGA_ROM_HEADER(9999, 0);
