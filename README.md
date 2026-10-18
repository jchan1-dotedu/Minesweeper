# Minesweeper

A classic Minesweeper clone written in C++ with [SFML](https://www.sfml-dev.org/) 2.5.1. Enter your name on the welcome screen, then clear the board without hitting a mine.

## Features

- Welcome screen with name entry (letters only, auto-capitalized)
- Left-click to reveal tiles, right-click to place or remove flags
- Reveals neighboring tiles automatically when you click an empty one
- Mine counter and in-game timer
- Face button to reset the game (changes when you win or lose)
- Debug button to show/hide mines
- Pause/play button (hides the board while paused)
- Leaderboard button

## Requirements

- C++ compiler with C++17 support (developed with G++ / MinGW)
- [CMake](https://cmake.org/) 3.16 or newer
- SFML 2.5.1 (graphics, window, system, audio)

## Project Layout

```
.
├── CMakeLists.txt
├── main.cpp          # Welcome screen, reads config, launches the game
├── game.hpp          # Tile and Game classes (game logic and rendering)
├── game.cpp
└── ../files/         # Must sit one level above the build folder
    ├── config.cfg
    ├── font.ttf
    └── images/       # tile, flag, mine, number, face, button, and digit images
```

## Configuration

`files/config.cfg` has three lines, one number per line:

```
25    <- columns
16    <- rows
50    <- number of mines
```

The window is sized from these values (32px per tile, plus 100px at the bottom for the buttons and counters).

## Building

1. Open `CMakeLists.txt` and change `SFML_DIR` to where SFML is installed on your machine. It should point to the folder containing `SFMLConfig.cmake`, for example:

   ```cmake
   set(SFML_DIR "C:/path/to/SFML-2.5.1/lib/cmake/SFML")
   ```

2. Configure and build from a `build` folder:

   ```bash
   mkdir build
   cd build
   cmake -G "MinGW Makefiles" ..
   cmake --build .
   ```

3. Copy the SFML DLLs (from SFML's `bin` folder) next to `minesweeper.exe` if Windows can't find them when you run it.

## Running

Run the executable **from inside the `build` folder**. The game loads its assets using relative paths like `../files/config.cfg`, so it won't find them if launched from somewhere else.

```bash
cd build
./minesweeper
```

## Controls

| Action | Input |
| --- | --- |
| Reveal tile | Left-click |
| Flag / unflag tile | Right-click |
| Reset game | Click the face |
| Show/hide mines | Debug button |
| Pause/resume | Pause/play button |
| Open leaderboard | Leaderboard button |

## Author

Joseph Chan, UF section 20682 (jchan1@ufl.edu)
Windows 11 x64, G++, SFML 2.5.1, Visual Studio Code
