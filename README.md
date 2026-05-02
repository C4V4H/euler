# Euler: Terminal 3D Renderer

A lightweight C project designed to render rotating 3D objects (such as cubes and tori) directly in the terminal using ASCII characters.

## Features

- **3D Math Library**: Custom implementation of 2D/3D vectors, quaternions for smooth rotations, and perspective projection.
- **Z-Buffering**: A depth buffer implementation to handle overlapping surfaces correctly.
- **ASCII Shading**: Real-time lighting calculation based on surface normals and dot product illumination.
- **TUI (Terminal User Interface)**: Managed raw mode terminal handling with flicker-free animations and terminal size auto-detection.

## Project Structure

- **src/**: Contains all source files (.c) and headers (.h).
- **obj/**: Temporary directory for object files (.o).
- **bin/**: Destination for the compiled executable.

## Prerequisites

- **Compiler**: clang.
- **Environment**: Unix-like system (Linux/macOS) for terminal control headers like `termios.h` and `sys/ioctl.h`.

## Getting Started

### Build and Run
To compile and run the project immediately in debug mode:
```bash
make run
```

### Build Targets
- **make**: Compiles the debug version with AddressSanitizer.
- **make production**: Compiles an optimized version with -O3 and native optimizations.
- **make clean**: Removes build artifacts (obj/ and bin/).
- **make rebuild**: Performs a clean build.
- **make prod-run**: Compiles the production version and runs it.

## Controls

- Press **q** or **Ctrl+Q** to exit the application.

## Implementation Details

The renderer uses a back-buffer system and a Z-buffer to ensure pixels are drawn in the correct depth order. Perspective is achieved by projecting 3D coordinates onto a 2D plane. The project utilizes ANSI escape codes to handle frame updates efficiently without full-screen flickers.
