# Build on macOS (SDCC + devkitSMS + SMSlib)

## Prereqs

- SDCC installed and on PATH
- devkitSMS checked out locally (example path used below):
  - `/Users/andreacollet/devkitSMS`
- `ihx2sms` built (local C build is OK on macOS)

## Tools

- `sdcc` (Z80 backend)
- `ihx2sms` (converts `.ihx` -> `.sms`)
- `crt0_sms.rel` from `devkitSMS/crt0`
- `SMSlib.lib` from `devkitSMS/SMSlib` (built manually)

## Known important compiler flags

SMSlib expects:

- `-mz80`
- `--reserve-regs-iy`
- `--peep-file .../SMSlib/src/peep-rules.txt`
- include paths:
  - `-I.../devkitSMS/SMSlib`
  - `-I.../devkitSMS/SMSlib/src`

## Make pipeline (concept)

1. Compile:
   - `sdcc <CFLAGS> -c src/main.c -o build/main.rel`
2. Link:
   - `sdcc -mz80 --no-std-crt0 crt0_sms.rel build/main.rel SMSlib.lib -o build/rom.ihx`
3. Convert:
   - `ihx2sms build/rom.ihx build/rom.sms`

## Output validation

- ROM size typically 32 KB.
- ROM footer should contain `TMR SEGA` and checksum should be updated by ihx2sms.

Example command to sanity-check:

```bash
stat -f "%N -> %z bytes (mtime %Sm)" build/dvd_bouncer.sms
hexdump -Cv build/dvd_bouncer.sms | tail -n 8
```

Expected tail includes:

`- 54 4d 52 20 53 45 47 41` → `TMR SEGA`
