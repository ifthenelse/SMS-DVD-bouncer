#include <stdbool.h>
#include <stdint.h>
#include "SMSlib.h"
#include "dvd_logo.h"

/* Screen geometry in tiles and pixels. */
#define SCREEN_TILE_W 32
#define SCREEN_TILE_H 24
#define SCREEN_PIXEL_W 256
#define SCREEN_PIXEL_H 192

/* Sprite tile placement and movement bounds for the DVD logo. */
#define LOGO_TILE_BASE 128
#define LOGO_PIXEL_W (DVD_LOGO_TILE_W * 8)
#define LOGO_PIXEL_H (DVD_LOGO_TILE_H * 8)
#define LOGO_MAX_X (SCREEN_PIXEL_W - LOGO_PIXEL_W)
#define LOGO_MAX_Y (SCREEN_PIXEL_H - LOGO_PIXEL_H)

/* Background and sprite palette indices used by the demo. */
#define TILE_EMPTY 0
#define PALETTE_BG 0
#define PALETTE_LOGO 1

/* Timings for movement cadence and corner flash effect. */
#define HEARTBEAT_PERIOD 24
#define MOVE_PERIOD 2
#define FLASH_FRAMES 14

/* PSG noise channel values used for the wall-hit "thud". */
#define WALL_HIT_SOUND_FRAMES 10
#define PSG_NOISE_LATCH 0xE0
#define PSG_NOISE_WHITE_DIV2048 0x06
#define PSG_CH3_VOL_LATCH 0xF0
#define PSG_VOL_SILENT 0x0F

/* Direct access to PSG I/O port. */
__sfr __at (0x7F) PSGPort;

/* Palette cycle for the bouncing logo. */
static const unsigned char logo_colors[] = {
    RGB(3, 3, 3),
    RGB(3, 0, 0),
    RGB(0, 3, 0),
    RGB(0, 0, 3),
    RGB(3, 3, 0),
    RGB(3, 0, 3),
    RGB(0, 3, 3)};

#define LOGO_COLOR_COUNT 7

/* Enqueue the full logo as a metasprite made of 8x8 hardware sprites. */
static void draw_logo_sprites(unsigned char x, unsigned char y) {
    unsigned char tx;
    unsigned char ty;
    unsigned int tile;

    /* Iterate all tiles in the 2D logo atlas and add one sprite per tile. */
    for (ty = 0; ty < DVD_LOGO_TILE_H; ++ty) {
        for (tx = 0; tx < DVD_LOGO_TILE_W; ++tx) {
            tile = LOGO_TILE_BASE + (ty * DVD_LOGO_TILE_W) + tx;
            SMS_addSprite(x + (tx * 8), y + (ty * 8), tile);
        }
    }
}

/* Start a short noise burst when the logo touches any wall. */
static void start_wall_hit_sound(unsigned char *sound_timer) {
    PSGPort = PSG_NOISE_LATCH | PSG_NOISE_WHITE_DIV2048;
    PSGPort = PSG_CH3_VOL_LATCH | 0x02;
    *sound_timer = WALL_HIT_SOUND_FRAMES;
}

/* Decay and stop the wall-hit sound over a few frames. */
static void update_wall_hit_sound(unsigned char *sound_timer) {
    /* Exit quickly when no sound is active. */
    if (*sound_timer == 0) {
        return;
    }

    /* Step the envelope timer down once per frame. */
    --(*sound_timer);

    /* Lower the volume in stages, then silence the channel. */
    if (*sound_timer == 7) {
        PSGPort = PSG_CH3_VOL_LATCH | 0x05;
    } else if (*sound_timer == 4) {
        PSGPort = PSG_CH3_VOL_LATCH | 0x08;
    } else if (*sound_timer == 0) {
        PSGPort = PSG_CH3_VOL_LATCH | PSG_VOL_SILENT;
    }
}

void main(void) {
    unsigned char x;
    unsigned char y;

    /* Runtime state for position, velocity, palette cycling, and effects. */
    int16_t logo_x = 96;
    int16_t logo_y = 80;
    signed char velocity_x = 1;
    signed char velocity_y = 1;
    unsigned char color_index = 0;
    unsigned char heartbeat_counter = 0;
    unsigned char move_counter = 0;
    unsigned char wall_hit_sound_timer = 0;
    bool heartbeat_on = false;
    unsigned char flash_timer = 0;

    /* Configure VDP and load logo sprite graphics before enabling display. */
    SMS_displayOff();
    SMS_useFirstHalfTilesforSprites(1);
    SMS_setSpriteMode(SPRITEMODE_NORMAL);
    SMS_VRAMmemsetW(0, 0x0000, 16384);
    SMS_load1bppTiles(dvd_logo_1bpp, LOGO_TILE_BASE, sizeof(dvd_logo_1bpp), 0, PALETTE_LOGO);

    /* Initialize background and sprite palettes, and silence PSG noise. */
    SMS_setBGPaletteColor(PALETTE_BG, RGB(0, 0, 0));
    SMS_setSpritePaletteColor(0, RGB(0, 0, 0));
    SMS_setSpritePaletteColor(PALETTE_LOGO, logo_colors[color_index]);
    PSGPort = PSG_CH3_VOL_LATCH | PSG_VOL_SILENT;

    /* Clear the background tilemap so sprites render over a clean field. */
    for (y = 0; y < SCREEN_TILE_H; ++y) {
        for (x = 0; x < SCREEN_TILE_W; ++x) {
            SMS_setTileatXY(x, y, TILE_EMPTY);
        }
    }

    /* Draw the first frame and turn the display on. */
    SMS_initSprites();
    draw_logo_sprites((unsigned char)logo_x, (unsigned char)logo_y);
    SMS_copySpritestoSAT();
    SMS_displayOn();

    /* Main loop: tick once per VBlank and update movement/effects. */
    for (;;) {
        bool hit_x = false;
        bool hit_y = false;

        /* Synchronize to VBlank for stable rendering timing. */
        SMS_waitForVBlank();

        /* Toggle a subtle heartbeat color to prove frame progression. */
        ++heartbeat_counter;
        if (heartbeat_counter >= HEARTBEAT_PERIOD) {
            heartbeat_counter = 0;
            heartbeat_on = !heartbeat_on;
        }

        /* Apply corner flash while active, otherwise show heartbeat backdrop. */
        if (flash_timer > 0) {
            --flash_timer;
            SMS_setBGPaletteColor(PALETTE_BG, RGB(3, 3, 0));
        } else {
            SMS_setBGPaletteColor(PALETTE_BG, heartbeat_on ? RGB(0, 0, 1) : RGB(0, 0, 0));
        }

        /* Advance and decay the currently playing wall-hit sound. */
        update_wall_hit_sound(&wall_hit_sound_timer);

        /* Move only on configured cadence to control speed. */
        ++move_counter;
        if (move_counter < MOVE_PERIOD) {
            /* Even on non-move frames, refresh SAT from current position. */
            SMS_initSprites();
            draw_logo_sprites((unsigned char)logo_x, (unsigned char)logo_y);
            SMS_copySpritestoSAT();
            continue;
        }
        move_counter = 0;

        /* Update position by one step on each axis for 45° movement. */
        logo_x += velocity_x;
        logo_y += velocity_y;

        /* Clamp and bounce on horizontal bounds. */
        if (logo_x < 0) {
            logo_x = 0;
            velocity_x = 1;
            hit_x = true;
        } else if (logo_x > LOGO_MAX_X) {
            logo_x = LOGO_MAX_X;
            velocity_x = -1;
            hit_x = true;
        }

        /* Clamp and bounce on vertical bounds. */
        if (logo_y < 0) {
            logo_y = 0;
            velocity_y = 1;
            hit_y = true;
        } else if (logo_y > LOGO_MAX_Y) {
            logo_y = LOGO_MAX_Y;
            velocity_y = -1;
            hit_y = true;
        }

        /* On any wall collision, cycle logo color and play hit sound. */
        if (hit_x || hit_y) {
            ++color_index;
            if (color_index >= LOGO_COLOR_COUNT) {
                color_index = 0;
            }
            SMS_setSpritePaletteColor(PALETTE_LOGO, logo_colors[color_index]);
            start_wall_hit_sound(&wall_hit_sound_timer);
        }

        /* On exact corner hits, trigger a short yellow flash. */
        if (hit_x && hit_y) {
            flash_timer = FLASH_FRAMES;
        }

        /* Render the new sprite positions into SAT for the next frame. */
        SMS_initSprites();
        draw_logo_sprites((unsigned char)logo_x, (unsigned char)logo_y);
        SMS_copySpritestoSAT();
    }
}

/* Embed a valid SEGA ROM header so emulators/hardware detect the ROM. */
SMS_EMBED_SEGA_ROM_HEADER(9999, 0);
