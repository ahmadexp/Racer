# Racer DS Folder

Demo video: [demo/racer_demo.mp4](demo/racer_demo.mp4)

This folder contains the Nintendo DSi/DS port for Racer that runs at 60 FPS on an actual DSi (and emulators, of course).

## What Is Here

- `Makefile`: BlocksDS ARM9 ROM build entry.
- `source/main.c`: DS gameplay/render/HUD entry source.
- `Media/tracktexture_ds.png`: DS-specific track texture used by the ROM/NitroFS.

## Dependencies

- BlocksDS toolchain (`BLOCKSDS` environment variable or default path in `Makefile`)

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
