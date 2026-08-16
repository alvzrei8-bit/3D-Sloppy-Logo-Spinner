# 3D ASCII Logo Spinner

A ~~high-performance~~ **sloppy but functional** 3D ASCII art logo spinner written in C++.

> ⚠️ **Warning**: This code is a mess. It was thrown together quickly and has zero polish. Use at your own risk.

## What It Does

- Spins ASCII/Unicode art logos in 3D (Y-axis only, because we're lazy)
- Renders with depth sorting (sometimes works, sometimes doesn't)
- Colors based on Z-depth (cyan/magenta vibes)
- Handles UTF-8 characters (after 3 rewrites)

## Code Quality

This code is:
- ✗ Not optimized (lies about `-O3` helping)
- ✗ Not memory efficient (uses `std::map` for rendering)
- ✗ Not maintainable (magic numbers everywhere)
- ✗ Not tested (we just tried it once and it worked)
- ✓ Does rotate ASCII art tho

## Building

Just throw this at your compiler and pray:

```bash
g++ -O3 -std=c++17 -o spin spin.cpp -lm
```

Or with clang (same vibes):
```bash
clang++ -O3 -std=c++17 -o spin spin.cpp -lm
```

## Usage

```bash
./spin logo.txt
```

Press `Ctrl+C` to stop.

## Logo File Format

The `logo.txt` file should contain ASCII or Unicode art. Any whitespace (spaces, tabs) will be treated as transparent (not rendered).

Supports:
- Regular ASCII characters
- Unicode box drawing: `┌─┐│└┘`
- Block drawing: `▄▀█▓░▒`
- Any UTF-8 characters!

Example `logo.txt`:
```
  _____ ____  ___   _ ___  _   _ _   _ 
 / ____|  _ \|_ _| | |_  \| | | | | | |
| |    | |_) || |  | | | | | | | |_| |
| |___ |  _ < | |  | | | | | |_|  _  |
 \____|_| \_\|___| |_|_|_| \___/|_| |_|
```

Example with blocks:
```
 ▄▄▄      
▒████▄    
▒██  ▀█▄  
░██▄▄▄▄██ 
```

## How It Works

1. **Loads the ASCII logo** from a text file
2. **Converts 2D coordinates** to 3D space (centered at origin)
3. **Applies 3D rotations** using rotation matrices
4. **Projects to 2D screen** using perspective projection
5. **Renders with depth** so characters properly occlude each other
6. **Colors by depth** - bright red/cyan for characters facing you, blue for those rotating away

## Color Mapping

The spinner uses depth-based coloring for extra visual pop:
- **Bright Red** - Closest to viewer (z > 0.8)
- **Bright Cyan** - Close (z > 0.5)
- **Bright Magenta** - Medium depth (z > 0.2)
- **Cyan** - Neutral (z > 0.0)
- **Magenta** - Far (z > -0.2)
- **Blue** - Farthest (z ≤ -0.2)

## Performance

Don't ask. It uses a `std::map` to store every pixel every frame. It's not optimized. It just... works. Sometimes.

## Rotation

Only does **Y-axis rotation** because we got tired and stopped there.

Want it faster? Edit this magic number:
```cpp
rotationY += 2.5f * deltaTime;  // go nuts
```

Want X or Z axis? Add it yourself, we're not your dev team.

## Known Issues

- Uses `std::map` for rendering (slow as hell)
- Perspective projection is janky
- Color cycling is arbitrary
- No error handling whatsoever
- Probably memory leaks somewhere
- Sometimes characters overlap wrong
- UTF-8 char handling is fragile

## Why Is This Code Bad?

- Built incrementally without planning
- No abstraction layers
- Magic numbers hardcoded everywhere
- No comments explaining logic
- Mixes concerns (rendering + math + I/O)
- Uses inefficient data structures
- No testing framework
- Perspective math is wrong but "good enough"

## Terminal Compatibility

Works on ANSI terminals because ANSI codes are universal. That's literally the only good decision made here.

## License

Do whatever you want with it. Honestly, we recommend not looking at it too closely.
