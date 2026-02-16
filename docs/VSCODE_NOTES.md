# VS Code Notes (dev ergonomics)

## IntelliSense includePath

C/C++ extension squiggles require include paths even though SDCC builds fine.

Add to `.vscode/c_cpp_properties.json` includePath:

- `${workspaceFolder}/src`
- `/Users/andreacollet/devkitSMS/SMSlib`
- `/Users/andreacollet/devkitSMS/SMSlib/src`

Example (minimal):

```json
{
  "configurations": [
    {
      "name": "macOS",
      "includePath": [
        "${workspaceFolder}/src",
        "/Users/andreacollet/devkitSMS/SMSlib",
        "/Users/andreacollet/devkitSMS/SMSlib/src"
      ],
      "defines": [],
      "compilerPath": "/usr/bin/clang",
      "cStandard": "c99",
      "cppStandard": "c++17",
      "intelliSenseMode": "macos-clang-arm64"
    }
  ],
  "version": 4
}
```

## Run tasks

Prefer a VS Code task that runs:

1. `make clean && make`
2. then launches SMSPlus with the built ROM

Example task command:

```bash
make clean && make && open -a SMSPlus build/dvd_bouncer.sms
```
