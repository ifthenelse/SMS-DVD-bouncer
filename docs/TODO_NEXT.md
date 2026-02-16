# TODO Next

## Current status

- [x] Confirm SMSlib frame API in installed devkit:
  - `SMS_waitForVBlank()` exists
  - `SMS_setFrameInterruptHandler(...)` exists
  - `SMS_doVBlankProcess()` does not exist
- [x] Add frame-tick proof (palette entry 0 heartbeat in `src/main.c`)
- [x] Implement BG tile rectangle path (solid tile index 1, 4x2 draw, bounce+color change)
- [x] Corner event implemented (flash + PSG beep)

## Next engineering steps

- [ ] Validate runtime in SMSPlus first:
  - Confirm movement is continuous
  - Confirm heartbeat toggle cadence
  - Confirm bounce color changes and corner flash/beep
- [ ] Validate in RetroArch second (`Genesis Plus GX`, hardware forced to SMS)
- [ ] Tune constants if needed:
  - `HEARTBEAT_PERIOD`
  - `FLASH_FRAMES`
  - `BEEP_FRAMES`
- [ ] Optional: sprite version only after BG version is stable
