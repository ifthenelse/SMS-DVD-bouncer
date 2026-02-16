# Emulator workflow

## SMSPlus (/Applications/SMSPlus)

- Primary fast loop emulator.
- Symptom guide:
  - Solid blue screen: display is on, program likely running or stalled.
  - Solid black screen: display off or stuck before VDP init.

## Recommended launch:

```bash
open -a SMSPlus build/dvd_bouncer.sms
```

### RetroArch (/Applications/Retroarch)

Use Genesis Plus GX core.

Recommended settings when debugging SMS ROMs:

System hardware: Sega Master System

BIOS: Off (or consistent between runs)

Launch:

```bash
open -a RetroArch build/dvd_bouncer.sms
```

### Debugging rule

Always test on SMSPlus first, then validate behavior on RetroArch.
