#pragma once

#include <array>
#include <cstdint>

// Forward declarations: keeps <SDL.h> out of every file that includes this header.
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

/**
 * @brief SDL2 front-end: window, display texture and keyboard.
 *
 * Knows nothing about CChip8: it receives the framebuffer to draw and exposes the
 * state of the 16 CHIP-8 keys. The main loop decides when to call each method.
 */
class CSdlFrontend
{
public:
    static constexpr int DISPLAY_WIDTH = 64;
    static constexpr int DISPLAY_HEIGHT = 32;
    static constexpr int KEY_COUNT = 16;

    using Display = std::array<bool, DISPLAY_WIDTH * DISPLAY_HEIGHT>;
    using Keys = std::array<bool, KEY_COUNT>;

    CSdlFrontend() = default;
    ~CSdlFrontend();

    // Owns SDL resources: copying would destroy them twice.
    CSdlFrontend(const CSdlFrontend&) = delete;
    CSdlFrontend& operator=(const CSdlFrontend&) = delete;

    /**
     * @brief Initializes SDL and creates the window, renderer and texture.
     * @param _sTitle Window title.
     * @param _iScale Initial window size multiplier (64x32 * _scale). The window is resizable.
     * @param _bVsync If true, render() waits for the monitor's vertical refresh.
     * @return True on success; on failure the error is printed to stderr.
     */
    bool init(const char* _sTitle, int _iScale, bool _bVsync);

    /**
     * @brief Processes pending window and keyboard events. Call once per frame.
     * @return False when the user asked to quit (window closed or Escape), true otherwise.
     */
    bool processEvents();

    /**
     * @brief Uploads the framebuffer to the texture and presents it on screen.
     * @param _display CHIP-8 framebuffer, row-major, true = pixel on.
     */
    void render(const Display& _display);

    /**
     * @brief Current state of the CHIP-8 keypad (index 0x0 to 0xF), true = pressed.
     */
    const Keys& getKeys() const { return m_aKeys; }

private:
    /**
     * @brief Maps a physical key to a CHIP-8 key.
     * @return The CHIP-8 key (0x0-0xF), or -1 if the key is not part of the keypad.
     */
    static int mapScancode(int _scancode);

    SDL_Window* m_pWindow = nullptr;
    SDL_Renderer* m_pRenderer = nullptr;
    SDL_Texture* m_pTexture = nullptr;
    bool m_bSdlInitialized = false;

    std::array<uint32_t, DISPLAY_WIDTH * DISPLAY_HEIGHT> m_aPixels = {}; // ARGB8888 staging buffer
    Keys m_aKeys = {};
};
