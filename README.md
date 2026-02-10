# mio-mod-template
A mod template for MIO: Memories in Orbit

## Prerequisites

- CMake 3.20 or higher
- MSVC (Visual Studio 2019 or later)
- vcpkg
- Ninja
  

## Building

### Using CMake Presets (Recommended)
```powershell
# Configure
cmake --preset x64-release

# Build
cmake --build --preset x64-release
```

### Running in Game

1. Copy `build/x64-release/bin/my_mod.dll` into your MIO install's `mods` directory
2. Launch the game