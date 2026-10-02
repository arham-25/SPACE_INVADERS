# Space Invaders

A C-based arcade shooter built with **raylib**.

## Creators

-   **ABIR AL ARHAM CHOWDHURY**
-   **HABIB UN NABI SEJAN**

## About the Game

Space Invaders is a classic-style arcade shooting game. Move your ship
along the bottom of the screen, shoot the incoming alien formation, and
avoid enemy fire. Choose **Easy**, **Medium**, or **Hard** difficulty,
protect yourself with barriers where available, and try to earn a place
on the leaderboard.

The game starts with three lives. Enemies move across the screen,
descend when they reach an edge, and become faster as time passes. Clear
enemy sets to continue; the game is endless and has no final winning
condition.

## Features

-   Main menu, player-name entry, and difficulty selection
-   Three difficulty modes: Easy, Medium, and Hard
-   Animated player and enemy sprites
-   Player and enemy projectiles
-   Protective walls (mode-dependent)
-   Score, lives, and a local top-10 leaderboard
-   Music and sound settings, including volume and mute controls
-   Pause and resume

## Requirements

### macOS

-   A Mac running a version of macOS supported by your installed raylib
    release
-   Visual Studio Code
-   A C compiler, such as Clang (available through Xcode Command Line
    Tools)
-   raylib installed and configured for your compiler
-   The game's complete project folder, including the `assets/`
    directory

### Windows

-   A Windows PC supported by your raylib build
-   Visual Studio Code
-   A C compiler/toolchain, such as MinGW-w64 GCC
-   raylib installed and configured for that toolchain
-   The game's complete project folder, including the `assets/`
    directory

**Important:** Keep the `assets/` folder beside the source file when
running the game. The program loads images, sound effects, and music
from paths such as `assets/player.png` and `assets/menu_music.mp3`. It
also reads/writes `leaderboard.txt` in the program's working directory.

## Run in VS Code

### 1. Set up the project

1.  Install VS Code and a C toolchain for your operating system.
2.  Install raylib using the instructions for your operating system and
    compiler.
3.  Open the **project folder** in VS Code (not just `main.c`).
4.  Confirm the folder contains `main.c` and `assets/`.

### 2. macOS

Install Apple's command-line developer tools if needed:

``` bash
xcode-select --install
```

Compile from the VS Code terminal, from the project folder:

``` bash
clang main.c -o space_invaders -lraylib -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
```

Then run:

``` bash
./space_invaders
```

If raylib is installed in a nonstandard location, add the appropriate
include and library paths (for example, `-I/path/to/include` and
`-L/path/to/lib`) before the raylib linker options.

### 3. Windows (MinGW-w64 GCC)

Open the VS Code terminal in the project folder. The exact
include/library paths depend on where you installed raylib. A typical
command is:

``` powershell
gcc main.c -o space_invaders.exe -I"path\to\raylib\include" -L"path\to\raylib\lib" -lraylib -lopengl32 -lgdi32 -lwinmm
```

Replace the example paths with the actual raylib paths for your
installation. Then run:

``` powershell
.\space_invaders.exe
```

Make sure the raylib runtime files required by your particular build
(such as its DLL, if applicable) can be found when launching the game.

## Controls

  Key            Action
  -------------- -------------------------------------------------------------
  Left Arrow     Move left
  Right Arrow    Move right
  Space          Shoot
  P              Pause / resume during gameplay
  1 / 2 / 3      Select menu options or difficulty, as shown on screen
  4              Open Settings from the main menu
  5              Exit from the main menu
  Enter          Confirm name entry / toggle selected audio setting
  Backspace      Delete a character while entering your name
  Up / Down      Select a setting
  Left / Right   Adjust the selected volume
  Escape         Go back or return to the main menu, depending on the screen
  R              Restart after game over

Follow the on-screen prompts for menu navigation. Some screens display
"0 to go back"; the current input handling may use Escape instead, so
Escape is the reliable back key.

## Gameplay

1.  Start from the main menu and enter a player name.
2.  Choose Easy, Medium, or Hard.
3.  Move left and right and press Space to fire at enemies.
4.  Avoid enemy bullets and prevent enemies from reaching your ship.
5.  Earn **10 points** for each enemy destroyed. The game ends when your
    three lives are lost or an enemy reaches the player area.
6.  Your score is added to the local leaderboard when the game-over
    screen is reached.

## Troubleshooting

-   **`raylib.h` not found:** raylib's include directory is not
    configured for the compiler. Install raylib and add the correct
    include path.
-   **`pkg-config: command not found`:** the `pkg-config` utility is not
    installed or isn't on your PATH. The macOS command above does not
    require `pkg-config` when raylib is configured in standard
    locations.
-   **The old version still runs:** rebuild after changing `main.c`,
    then run the newly compiled executable. Check that VS Code's task
    builds the same folder and output filename that your launch
    configuration runs.
-   **Images or audio are missing:** run from the project folder so the
    relative `assets/...` paths resolve correctly.

## Credits

Game creators: **ABIR AL ARHAM CHOWDHURY** and **HABIB UN NABI SEJAN**.

## THANKS FOR PLAYING
