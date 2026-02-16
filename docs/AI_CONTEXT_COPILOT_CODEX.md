# AI Context (for Codex / GitHub Copilot)

Use this as "project context" so the assistant does not re-try already failed directions.

## Project

Sega Master System homebrew ROM in C (SDCC + devkitSMS + SMSlib). Output is a `.sms` ROM built via `.ihx` + `ihx2sms`.

## Goals

Implement a "DVD screensaver" rectangle:

- 32×16 px rectangle, moves diagonally (dx=±1, dy=±1)
- Bounces on borders; changes color on each bounce
- Corner hit: background flash yellow + short PSG beep

## Environment

- macOS
- devkitSMS path example: `/Users/andreacollet/devkitSMS`
- Emulators:
  - `/Applications/SMSPlus`
  - `/Applications/Retroarch` (Genesis Plus GX)

## Toolchain constraints (must respect)

- SDCC flags required for SMSlib:
  - `-mz80 --reserve-regs-iy`
  - `--peep-file <devkitSMS>/SMSlib/src/peep-rules.txt`
  - `-I<devkitSMS>/SMSlib -I<devkitSMS>/SMSlib/src`
- Link with:
  - `<devkitSMS>/crt0/crt0_sms.rel`
  - `<devkitSMS>/SMSlib/SMSlib.lib`
- Convert `.ihx -> .sms` using built `ihx2sms`

## What is already known

- ROM builds and contains `TMR SEGA` header (checksum updated).
- A basic "set backdrop blue" ROM displays correctly in SMSPlus.
- Some SMSlib APIs suggested elsewhere are NOT available here:
  - `SMS_setVBlankInterruptHandler(...)` failed compile (implicit declaration / wrong signature).
  - `SMS_doVBlankProcess()` is not present in this SMSlib version.
- Verified available frame APIs in this toolchain:
  - `SMS_waitForVBlank()`
  - `SMS_setFrameInterruptHandler(...)`
- Current `src/main.c` already implements BG rectangle movement + bounce color + corner flash/beep.

## Critical Implementation Guidance

- Avoid using nonexistent SMSlib functions.
- Use a simple frame loop based on `SMS_waitForVBlank()` once per iteration.
- Keep `SMS_setBackdropColor(...)` usage correct:
  - It takes a palette entry index (0-15), not an `RGB(...)` color value.
- Use `SMS_setBGPaletteColor(...)` / `SMS_setSpritePaletteColor(...)` when setting actual colors.

## Coding plan (recommended)

1. Implement a minimal "frame counter toggles BG palette 0 between blue/yellow" to validate a working per-frame tick.
2. Implement rectangle in BG tilemap (most robust).
3. Add color changes by altering palette entry used by the rectangle’s solid tile.
4. Add corner flash by toggling backdrop palette entry (or BG palette 0).
5. Add corner beep using PSG port 0x7F (tone + volume with short duration).

## Debug symptoms mapping

- Blue screen only: display on, but no per-frame tick or tilemap writes not happening as expected.
- Black screen: display off or early crash (bad VDP init, wrong wait, or interrupt misuse).
- No sprite visible: often tile index/palette mismatch or SAT not committed; defer until BG version works.

## Deliverable expectation

Produce a working ROM first with BG tilemap rectangle + corner flash/beep, then optionally migrate to sprites.
