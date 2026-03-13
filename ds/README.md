# Racer DSi/DS port

<img width="960" height="540" alt="Image" src="https://github.com/user-attachments/assets/de714941-7dae-4a70-a1b5-602c82f7857b" />

This folder contains the Nintendo DSi/DS port for Racer that runs at 60 FPS on an actual DSi (and emulators, of course).

## What Is Here

- `Makefile`: BlocksDS ARM9 ROM build entry.
- `source/main.c`: DS gameplay/render/HUD entry source.
- `Media/tracktexture_ds.png`: DS-specific track texture used by the ROM/NitroFS.

## Dependencies

- [BlocksDS](https://blocksds.skylyrac.net/) toolchain (`BLOCKSDS` environment variable or default path in `Makefile`)

## Build

From this folder:

```sh
make
```

Optional toolchain override:

```sh
make BLOCKSDS=/path/to/blocksds/core
```

The generated ROM output is produced by the BlocksDS default ARM9 makeflow (for this project, typically `racer_dsi.nds`).

## Emulation Recommendation

Use melonDS for emulation and quick iteration/testing. Maybe DSi Mode for faster performance.
