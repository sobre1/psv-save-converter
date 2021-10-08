# PSV Save Converter
[![Downloads](https://img.shields.io/github/downloads/bucanero/psv-save-converter/total.svg?maxAge=3600)](https://github.com/bucanero/psv-save-converter/releases)
[![Build binaries](https://github.com/bucanero/psv-save-converter/actions/workflows/build.yml/badge.svg)](https://github.com/bucanero/psv-save-converter/actions/workflows/build.yml)
[![Build Windows binaries](https://github.com/bucanero/psv-save-converter/actions/workflows/build-win.yml/badge.svg)](https://github.com/bucanero/psv-save-converter/actions/workflows/build-win.yml)
[![License](https://img.shields.io/github/license/bucanero/psv-save-converter.svg)](./LICENSE)

This tool converts and resigns PS1 and PS2 savegame files to PlayStation 3 `.PSV` save format.

The tool can also export `.PSV` PS3 files back to PS1 (`.mcs`) and PS2 (`.psu`) save formats.

## Supported formats

- `.mcs` : PS1 MCS File
- `.psx` : PS1 AR/GS/XP PSX File
- `.cbs` : PS2 CodeBreaker File
- `.max` : PS2 ActionReplay Max File
- `.xps` : PS2 Xploder/SharkPort File
- `.psu` : PS2 EMS File (uLaunchELF)
- `.psv` : PS3 PSV File (to PS1 `.mcs`/PS2 `.psu`)

## Usage

Drag and drop a PS1 or PS2 save file onto the program. It will convert and resign it so that you may use with your PS3 through USB.

Or use CMD:
```bash
	./psv-save-converter <savefile>
```

## Credits

- Resign code based on [ps3-psvresigner](https://github.com/dots-tb/ps3-psvresigner) by [@dots_tb](https://github.com/dots-tb)
- Savegame format code from [CheatDevicePS2](https://github.com/root670/CheatDevicePS2) and [PSV-Exporter](https://github.com/PMStanley/PSV-Exporter)
 
## License

`psv-save-resigner` is released under the [GPL-3.0 License](./LICENSE).
