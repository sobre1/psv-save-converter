# PSV Save Converter

This tool converts and resigns PS1 and PS2 savegame files to PlayStation 3 `.PSV` save format.

## Supported formats

- `.mcs` : PS1 MCS File
- `.max` : PS2 ActionReplay Max File
- `.psu` : PS2 EMS File (uLaunchELF)

## Usage

Drag and drop a modified PSV PSOne or PS2 save file onto the program. It will resign it so that you may use with your PS3 through USB.

Or use CMD:
```bash
	./psv-save-converter <savefile>
```

## Credits

- Resign code based on [ps3-psvresigner](https://github.com/dots-tb/ps3-psvresigner) by [@dots_tb](https://github.com/dots-tb)
- Savegame format code from [CheatDevicePS2](https://github.com/root670/CheatDevicePS2) and [PSV-Exporter](https://github.com/PMStanley/PSV-Exporter)
 
## License

`psv-save-resigner` is released under the [GPL-3.0 License](./LICENSE).
