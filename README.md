# Racer

A C++ 3D racing game engine built with SDL 1.2 and OpenGL 2.1.

## Overview

Racer is a lightweight 3D game engine designed for racing simulations. It utilizes SDL (Simple DirectMedia Layer) for window management and input handling, and OpenGL (Fixed Function Pipeline) for rendering. The engine handles 3D camera transformations, basic physics, texture loading (BMP and PNG via LodePNG), and HUD rendering.

## Technology Stack

*   **Language:** C++ (C++11 standard)
*   **Windowing & Input:** SDL 1.2 (compatible with `sdl12_compat` on modern systems)
*   **Rendering:** OpenGL 2.1 (Legacy/Fixed Function Pipeline)
*   **Build System:** Meson + Ninja
*   **Image Loading:** Built-in LodePNG (for PNG) and SDL (for BMP)

## Prerequisites

To build and run this project, you need the following installed on your system:

### macOS
*   **C++ Compiler:** Clang (via Xcode Command Line Tools)
*   **Build Tools:** `meson` and `ninja`
*   **Dependencies:** `sdl` or `sdl12_compat`, `pkg-config`

```bash
# Install dependencies via Homebrew
brew install meson ninja sdl12-compat pkg-config
```

### Linux (Ubuntu/Debian)
```bash
sudo apt-get install build-essential meson ninja-build libsdl1.2-dev libglu1-mesa-dev
```

## Building the Project

1.  **Configure the build directory:**
    ```bash
    meson setup build
    ```

2.  **Compile the project:**
    ```bash
    ninja -C build
    ```

## Running the Game

After a successful build, run the executable from the project root:

```bash
./build/Racer
```

*Note: Ensure you run the executable from the root directory so it can correctly locate the `Media/` folder and `settings.ini`.*

## Controls

*   **Arrow Keys / WASD:** Accelerate, Brake, Turn
*   **Space:** Handbrake / Action
*   **Esc:** Exit / Menu

## Project Structure

*   **`source/`**: Source code files.
    *   **`main.cpp`**: Application entry point and main loop.
    *   **`generic/`**: Core engine systems (OpenGL init, Input, Math, 2D/3D drawing functions).
        *   `initvideo.h`: OpenGL and SDL video initialization.
        *   `opengl/`: Texture loading and OpenGL wrappers.
        *   `sdldraw/`: Software rendering fallbacks and helpers.
    *   **`projectspecific/`**: Game-specific logic.
        *   `play.h`: Physics and player movement logic.
        *   `drawworld.h`: 3D world rendering loop.
        *   `drawhud.h`: UI and Heads-Up Display rendering.
        *   `textures.h`: Texture resource management.
*   **`Media/`**: Game assets (Textures, Images).
*   **`meson.build`**: Build configuration.
*   **`settings.ini`**: Runtime configuration (Resolution, Fullscreen, Antialiasing).

## Recent Fixes & Troubleshooting

### Black Screen on Startup
If you experience a black screen:
1.  **Backface Culling:** The engine now disables backface culling (`glDisable(GL_CULL_FACE)`) to ensure track geometry is visible from all angles.
2.  **OpenGL Context (macOS):** On macOS, the build is linked against the native Apple OpenGL Framework instead of Mesa to ensure correct context creation.
3.  **Multisampling:** `glEnable` calls for multisampling were moved *after* `SDL_SetVideoMode` to prevent initialization crashes on some drivers.

### Compilation Warnings
You may see warnings regarding "deprecated OpenGL functions". This is expected as the engine uses OpenGL 2.1 (Legacy) for simplicity and broad compatibility. These warnings can be ignored for now.
