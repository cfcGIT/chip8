# chip8

[![CI](https://github.com/cfcGIT/chip8/actions/workflows/ci.yml/badge.svg)](https://github.com/cfcGIT/chip8/actions/workflows/ci.yml)

CHIP-8 emulator developed in C++20 following [this guide](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/).

The goal of this project is to learn how an emulator works from the inside: the fetch-decode-execute loop, the original COSMAC VIP hardware and its quirks.

I use Claude Code as a tutor and code reviewer for the emulator core, which I write myself, and as a helper for the parts I already know (CMake, SDL, CI). The rules for that split are in [CLAUDE.md](CLAUDE.md).

The core has no SDL dependency: it exposes `step()`, input, display and state, so the same core can be driven by the SDL front-end, a headless test runner or Python.

## Roadmap

- [x] **IBM logo and first six opcodes:** Fetch-decode-execute loop, first six opcodes (`00E0`, `1NNN`, `6XNN`, `7XNN`, `ANNN`, `DXYN`), console renderer.
- [ ] **SDL2 front-end:** Window, scaled 64×32 display, keyboard input and sound.
- [ ] **Test suite:** Full instruction set, passing the [Timendus CHIP-8 test suite](https://github.com/Timendus/chip8-test-suite).
- [ ] **Automated tests in CI:** Run the test ROMs headless on Linux and Windows.
- [ ] **Games:** Pong, Tetris or Space Invaders playable.
- [ ] **Toolchain:** Own disassembler and assembler, plus a small game written with them.
- [ ] **Reinforcement learning preparation:** Expose the core via pybind11 for reinforcement learning experiments.

## Requirements

| OS | Install |
|----|---------|
| Fedora | `sudo dnf install cmake ninja-build gcc-c++ SDL2-devel` |
| Ubuntu 24.04+ | `sudo apt install cmake ninja-build g++ libsdl2-dev` |
| Windows | Visual Studio 2022+ with the *Desktop development with C++* workload |

On Windows, SDL2 is downloaded and built automatically by CMake.

## Test ROMs

ROMs are not included in the repository. Download them into `roms/`:

```bash
./scripts/fetch_roms.sh
```

On Windows, run it from Git Bash.

## Build and run

All commands are run from the project root. There are two presets: `debug` and `release` (optimized, with debug info). The executable ends up in `build/<preset>/`.

### Linux

```bash
./scripts/build.sh release                  # build
./build/release/chip8 roms/2-ibm-logo.ch8   # run
```

Other options:

```bash
./scripts/build.sh                                # debug build
./scripts/build.sh debug run roms/2-ibm-logo.ch8  # build and run in one step
./scripts/build.sh clean                          # delete build/
```

### Windows

Open the *Developer PowerShell for VS* (a regular PowerShell has no compiler in `PATH`):

```powershell
cmake --preset release                         # configure (first time only)
cmake --build --preset release                 # build
.\build\release\chip8.exe roms\2-ibm-logo.ch8  # run
```

Or open the folder in Visual Studio, pick a preset in the toolbar, set the ROM path under *Debug → Debug and Launch Settings for chip8.exe* (`"args"`) and press F5.

## Credits

- [Guide to making a CHIP-8 emulator](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/) by Tobias V. Langhoff.
- [CHIP-8 test suite](https://github.com/Timendus/chip8-test-suite) by Timendus.
- [CHIP-8 test ROM](https://github.com/corax89/chip8-test-rom) by corax89.
- BC_test ROM by BestCoder, via the [stianeklund/chip8](https://github.com/stianeklund/chip8) mirror.

## License

[MIT](LICENSE). Test ROMs are third-party and keep their authors' licenses.