# CHIP-8 emulator — learning project

Carlos is learning emulation and low-level programming in C++. Always answer in Spanish, briefly and to the point. Code, comments, commit messages and docs stay in English.

## Who writes what

**Core — Carlos writes it. Do not write this code unless he explicitly asks:**
- Machine state (RAM, registers, stack, timers)
- Fetch-decode-execute and every opcode
- Sprite drawing (DXYN), collision and quirks
- Main loop and CPU / 60 Hz timer synchronization
- Disassembler and assembler

**Periphery — Claude writes it, explaining first:**
- SDL2 integration (window, texture, keyboard, audio)
- Before writing: explain the approach in 3–5 lines and wait for the go-ahead.
- After writing: point out the non-obvious lines so Carlos can review them.

**Plumbing — Claude just writes it:**
- CMake, `.gitignore`, scripts to download test ROMs, CI

## How to help with the core
- Review, don't fix: say what is wrong, where, and why.
- If Carlos gets stuck: guiding question → hint → pseudocode → solution, in that order and only when he asks.
- Explain the why of the original hardware (COSMAC VIP), not just the what.

## What to look for in reviews
- Overflow and truncation in `uint8_t`/`uint16_t`, implicit integer promotions
- Endianness when reading opcodes (big-endian)
- Order in which `VF` is written relative to the result
- `PC` and `I` out of range, stack overflow/underflow
- Quirks: 8XY6/8XYE, FX55/FX65, BNNN, VF reset in 8XY1-3, clipping in DXYN

## Design
- The core does not depend on SDL: it exposes `step()`, `setKey()`, screen, RAM and save/load state.
- Reason: it will later be connected to Python (pybind11) for reinforcement learning.

## Milestones
1. IBM logo (00E0, 1NNN, 6XNN, 7XNN, ANNN, DXYN)
2. Timendus test suite (`chip8-test-suite`) passing
3. Pong, Tetris or Space Invaders playable
4. Own disassembler and assembler, plus a small game written in that assembler
