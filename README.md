Docs [here](https://github.com/mateusluizfb/puzzle-horror-game-IDJ/blob/main/DOCS.md)


## Instructions

### Linux build (native)

1. Install `g++` and the SDL2 dev libraries:
   ```bash
   # Fedora
   sudo dnf install SDL2-devel SDL2_image-devel SDL2_mixer-devel SDL2_ttf-devel
   # Ubuntu / Debian
   sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev
   # Arch
   sudo pacman -S sdl2 sdl2_image sdl2_mixer sdl2_ttf
   ```
2. Build and package:
   ```bash
   make all          # release build -> dist_linux/JOGO
   make debug        # debug build
   make clean        # remove generated files
   ```
3. Run:
   ```bash
   ./dist_linux/JOGO
   ```

### macOS build (native)

1. Install Xcode Command Line Tools and the SDL2 libraries via Homebrew:
   ```bash
   xcode-select --install
   brew install sdl2 sdl2_image sdl2_mixer sdl2_ttf
   ```
2. Build and package:
   ```bash
   make PLATFORM=macos all       # release build -> dist_macos/JOGO
   make PLATFORM=macos debug     # debug build
   make PLATFORM=macos clean     # remove generated files
   ```
3. Run:
   ```bash
   ./dist_macos/JOGO
   ```

### Windows build (cross-compile from Linux)

1. Install the MinGW-w64 cross-compiler:
   ```bash
   sudo apt install mingw-w64      # Debian / Ubuntu
   # or
   sudo dnf install mingw64-gcc mingw64-gcc-c++    # Fedora
   # or
   sudo pacman -S mingw-w64-gcc    # Arch
   ```
   The compiler used is `x86_64-w64-mingw32-g++`.

2. Download the MinGW (`x86_64-w64-mingw32`) variants of SDL2 and place them
   under `libs/windows/SDL2/` with this layout:
   ```
   libs/windows/SDL2/include/SDL2/   <- headers
   libs/windows/SDL2/lib/           <- *.a import libraries
   libs/windows/SDL2/bin/            <- *.dll runtime files
   ```
   Download from:
   - https://github.com/libsdl-org/SDL/releases
   - https://github.com/libsdl-org/SDL_image/releases
   - https://github.com/libsdl-org/SDL_mixer/releases
   - https://github.com/libsdl-org/SDL_ttf/releases

   From each archive, merge the `x86_64-w64-mingw32/{include,lib,bin}/`
   folders into `libs/windows/SDL2/`.

3. Build and package:
   ```bash
   make PLATFORM=win all       # release build -> dist_win/JOGO.exe
   make PLATFORM=win debug     # debug build (console window visible)
   make PLATFORM=win clean     # remove generated files
   ```

4. Run the bundled `dist_win/JOGO.exe` on a Windows machine. The `package`
   target copies the required SDL2 DLLs and MinGW runtime DLLs alongside the
   executable.
