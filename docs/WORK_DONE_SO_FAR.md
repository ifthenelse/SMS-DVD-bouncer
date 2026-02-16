# Work Done So Far (Timeline Summary)

## Toolchain & build

- Built `ihx2sms` on macOS via:
  - `cc -O2 -std=c99 ~/devkitSMS/ihx2sms/src/*.c -o ~/devkitSMS/ihx2sms/ihx2sms`
- Confirmed ROM builds to 32 KB with a valid header:
  - ihx2sms reported: `SEGA header found, checksum updated`

## SMSlib integration issues resolved

- Include path issue fixed:
  - `SMSlib.h` exists at: `devkitSMS/SMSlib/src/SMSlib.h`
  - Makefile must include `-I.../SMSlib/src`
- Ensured SMSlib compatibility flags:
  - `--reserve-regs-iy`
  - `--peep-file .../SMSlib/src/peep-rules.txt`

## Runtime behavior observations

- A simple background color program proved:
  - VDP display can turn on
  - ROM boots and runs far enough to show a blue screen
- Multiple attempts at sprite and BG animation produced:
  - Static background only (no movement)
  - Sometimes black screen when bypassing SMSlib incorrectly

## Key discovery (most important)

- Some suggested APIs do not exist in this SMSlib version:
  - `SMS_setVBlankInterruptHandler(...)` failed to compile (implicit declaration / too many parameters).
- `SMS_doVBlankProcess()` is also not available in this installed SMSlib build.
- The compatible frame model for this repo is a plain `SMS_waitForVBlank()` game loop (with optional `SMS_setFrameInterruptHandler(...)` if needed).

## Latest progress (2026-02-16)

- Confirmed directly in `~/devkitSMS/SMSlib/src/SMSlib.h`:
  - Available frame APIs are `SMS_waitForVBlank()` and `SMS_setFrameInterruptHandler(...)`.
  - `SMS_doVBlankProcess()` is not part of this SMSlib build.
- Replaced `src/main.c` with a BG tilemap implementation:
  - One solid tile at index `1`.
  - Rectangle drawn as `4x2` tiles.
  - Per frame: erase old rectangle, update position with bounce logic, draw new rectangle.
  - On bounce: cycle rectangle color through palette entry `1`.
  - On corner hit: backdrop flash (palette entry `15`) + short PSG channel 0 beep.
- Added frame-tick proof in the main loop:
  - Background palette entry `0` toggles every `N` frames.
  - If the color does not toggle, frame tick/VBlank progression is not happening.
