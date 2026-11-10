cmake_minimum_required(VERSION 3.28)
project(chip8 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- SDL2 ---------------------------------------------------------------------
# Prefer the system SDL2 (Linux: dnf install SDL2-devel). It is built with the
# distro's video/audio backends (Wayland, X11, PipeWire); building SDL from source
# on Linux silently drops any backend whose -devel headers are missing.
# Fallback (Windows, or no system package): fetch and build SDL2 statically.
find_package(SDL2 CONFIG QUIET)

if(SDL2_FOUND)
    message(STATUS "Using system SDL2 ${SDL2_VERSION}")
    set(CHIP8_SDL_TARGET SDL2::SDL2)
else()
    message(STATUS "System SDL2 not found: fetching SDL2 2.32.10")
    include(FetchContent)

    # Static library only: no need to copy SDL2.dll next to the .exe.
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        SDL2
        URL https://github.com/libsdl-org/SDL/releases/download/release-2.32.10/SDL2-2.32.10.tar.gz
        URL_HASH SHA256=5f5993c530f084535c65a6879e9b26ad441169b3e25d789d83287040a9ca5165
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(SDL2)
    set(CHIP8_SDL_TARGET SDL2::SDL2-static)
endif()

# --- Common options, only for our code (not for SDL) ---------------------------
add_library(chip8_options INTERFACE)
target_compile_options(chip8_options INTERFACE
    # /w44242 /w44254 /w44365: off-by-default MSVC conversion warnings, enabled at level 4
    # (roughly what -Wconversion -Wsign-conversion gives on GCC/Clang).
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive- /utf-8 /w44242 /w44254 /w44365>
    # -Wsign-conversion: not part of -Wconversion in C++; catches the same int -> unsigned cases as C4365.
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wconversion -Wsign-conversion>
)

# --- Core: no SDL ---------------------------------------------------------------
add_library(chip8_core STATIC
    src/core/chip8.cpp
    src/core/chip8.h)
target_include_directories(chip8_core PUBLIC src)
target_link_libraries(chip8_core PRIVATE chip8_options)

# --- Executable: core + SDL -----------------------------------------------------
add_executable(chip8
    src/app/main.cpp
    src/core/chip8.h)
target_link_libraries(chip8 PRIVATE
    chip8_core
    chip8_options
    ${CHIP8_SDL_TARGET}
)
# SDL2main only matters on Windows (it provides WinMain); some packages don't ship it.
if(TARGET SDL2::SDL2main)
    target_link_libraries(chip8 PRIVATE SDL2::SDL2main)
endif()
