#!/usr/bin/env bash
# Configures and builds the emulator with a CMake preset.
#
# Usage:
#   scripts/build.sh                        # debug build
#   scripts/build.sh release                # optimized build with debug info
#   scripts/build.sh debug run <ROM path>   # build, then run the emulator with ROM
#   scripts/build.sh clean                  # delete build/

set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

preset="${1:-debug}"

if [[ "$preset" == "clean" ]]; then
    rm -rf build
    echo "Removed build/"
    exit 0
fi

if [[ "$preset" != "debug" && "$preset" != "release" ]]; then
    echo "Unknown preset '$preset' (expected: debug, release, clean)" >&2
    exit 2
fi

# Configure only when needed: the first time, or after a clean.
# Later CMakeLists.txt edits are picked up automatically by the build step.
if [[ ! -f "build/$preset/build.ninja" ]]; then
    cmake --preset "$preset"
fi
cmake --build --preset "$preset"

if [[ "${2:-}" == "run" ]]; then
    rom="${3:?Usage: scripts/build.sh $preset run <rom>}"
    exec "build/$preset/chip8" "$rom"
fi
