# Statement of Work (SOW) — SMS DVD Bouncer

## Objective

Implement a Sega Master System ROM that displays a "DVD screensaver" style rectangle bouncing at 45°, changing color on border collisions, and triggering a corner effect (yellow flash + beep) on corner collisions.

## Constraints / Environment

- Compiler: SDCC targeting Z80 (`-mz80`)
- Library: devkitSMS + SMSlib
- Output: `.sms` ROM with `TMR SEGA` header and correct checksum
- Dev machine: macOS
- Emulators: RetroArch + SMSPlus

## Deliverables

1. **Stable main loop timing**:
   - A reliable once-per-frame tick.
   - Must not freeze on `SMS_waitForVBlank()` and must not depend on non-working APIs.
2. **Rendering path**:
   - Phase 1: Background tilemap rectangle (robust).
   - Phase 2 (optional): Sprite-based rectangle.
3. **Behavior**:
   - Rectangle starts at a random position (within bounds), moves diagonally (dx = ±1, dy = ±1).
   - On wall hit (X or Y): bounce and change rectangle color.
   - On corner hit (X and Y simultaneously): bounce, change color, background flash yellow for N frames, play beep.
4. **Audio**:
   - Short beep on PSG channel 0 (simple tone + volume envelope).
5. **Test plan**:
   - ROM runs identically in SMSPlus and RetroArch (Genesis Plus GX) after core configuration.

## Acceptance Criteria

- ROM boots consistently (no black screen) in SMSPlus and RetroArch.
- Rectangle visibly moves each frame.
- Color changes occur on all bounces.
- Corner collision triggers:
  - background flash to yellow and back
  - audible short beep
- No long-term corruption / freeze after 60+ seconds runtime.

## Implementation Notes (important)

- SMSlib version differences exist; avoid relying on functions that may not exist (e.g., `SMS_setVBlankInterruptHandler` caused compile errors: "too many parameters / implicit declaration").
- In this installed devkitSMS/SMSlib build, `SMS_doVBlankProcess()` is not available; use `SMS_waitForVBlank()` as the frame sync primitive.
- `--reserve-regs-iy` and the peep rules file are required for SMSlib compatibility.

```

```
