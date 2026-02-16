# SMS DVD Bouncer (Sega Master System)

Small Sega Master System demo: a DVD-like rectangle bounces at 45° across the screen, changes color on wall hits, and on corner hits it flashes the background yellow and plays a short "blink" sound.

This repo is being developed on macOS with **SDCC + devkitSMS + SMSlib** and tested on:

- `/Applications/Retroarch` (recommended core: Genesis Plus GX)
- `/Applications/SMSPlus`

## Current status (high-level)

- Toolchain on macOS works: `.sms` ROM produced (32 KB) with valid `TMR SEGA` header.
- Baseline video output confirmed: can show a solid blue screen.
- SMSlib API verified for this setup:
  - Available: `SMS_waitForVBlank()` and `SMS_setFrameInterruptHandler(...)`.
  - Not available: `SMS_doVBlankProcess()` / `SMS_setVBlankInterruptHandler(...)`.
- `src/main.c` now implements a BG-tile DVD bouncer:
  - 4x2 tile rectangle (32x16 px), per-frame erase/move/draw loop
  - color change on wall hits
  - corner event: backdrop flash + short PSG beep
  - frame-tick proof: periodic background palette toggle
- Pending: runtime validation in both emulators (SMSPlus, then RetroArch Genesis Plus GX in SMS mode).

## Goals

1. Validate behavior in SMSPlus and RetroArch.
2. Tune movement speed/flash/beep timing after emulator verification.
3. Optional follow-up: sprite-based version after BG path is confirmed stable.

## Quick build & run

```bash
make clean && make
open -a SMSPlus build/dvd_bouncer.sms
# or: open -a RetroArch build/dvd_bouncer.sms
```

## Emulator notes

### SMSPlus

- Good for fast iteration.
- Use the freshly built `build/dvd_bouncer.sms`.

### RetroArch (Genesis Plus GX)

- Ensure it runs as Sega Master System (not Game Gear).
- Disable BIOS if you see weird boot differences.
- Test after confirming SMSPlus behavior.

## Repository structure (suggested)

- `src/main.c` — main ROM code
- `build/` — build outputs
- `docs/` — engineering notes and SOW
- `Makefile` — SDCC + linker + ihx2sms pipeline
