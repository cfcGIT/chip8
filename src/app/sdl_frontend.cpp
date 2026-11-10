#include "sdl_frontend.h"

#include <iostream>

#include <SDL.h>

namespace
{
    // Pixel colors (ARGB8888). The original VIP drew white-ish pixels on black.
    constexpr uint32_t COLOR_ON = 0xFFE8E8E8;
    constexpr uint32_t COLOR_OFF = 0xFF101010;

    /*
     * COSMAC VIP hex keypad        Mapped to (physical positions, any layout)
     *   1 2 3 C                      1 2 3 4
     *   4 5 6 D                      Q W E R
     *   7 8 9 E                      A S D F
     *   A 0 B F                      Z X C V
     */
    struct KeyBinding
    {
        SDL_Scancode scancode;
        int chip8Key;
    };

    constexpr KeyBinding KEY_BINDINGS[] = {
        { SDL_SCANCODE_1, 0x1 }, { SDL_SCANCODE_2, 0x2 }, { SDL_SCANCODE_3, 0x3 }, { SDL_SCANCODE_4, 0xC },
        { SDL_SCANCODE_Q, 0x4 }, { SDL_SCANCODE_W, 0x5 }, { SDL_SCANCODE_E, 0x6 }, { SDL_SCANCODE_R, 0xD },
        { SDL_SCANCODE_A, 0x7 }, { SDL_SCANCODE_S, 0x8 }, { SDL_SCANCODE_D, 0x9 }, { SDL_SCANCODE_F, 0xE },
        { SDL_SCANCODE_Z, 0xA }, { SDL_SCANCODE_X, 0x0 }, { SDL_SCANCODE_C, 0xB }, { SDL_SCANCODE_V, 0xF },
    };
}

CSdlFrontend::~CSdlFrontend()
{
    // Destroy in reverse order of creation.
    if (m_pTexture)
    {
        SDL_DestroyTexture(m_pTexture);
    }
    if (m_pRenderer)
    {
        SDL_DestroyRenderer(m_pRenderer);
    }
    if (m_pWindow)
    {
        SDL_DestroyWindow(m_pWindow);
    }
    if (m_bSdlInitialized)
    {
        SDL_Quit();
    }
}

bool CSdlFrontend::init(const char* _sTitle, int _iScale, bool _bVsync)
{
    // main() is our own entry point (SDL_MAIN_HANDLED in CMake), so tell SDL it may initialize.
    SDL_SetMainReady();

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }
    m_bSdlInitialized = true;

    m_pWindow = SDL_CreateWindow(_sTitle,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        DISPLAY_WIDTH * _iScale, DISPLAY_HEIGHT * _iScale,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!m_pWindow)
    {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        return false;
    }

    Uint32 rendererFlags = SDL_RENDERER_ACCELERATED;
    if (_bVsync)
    {
        rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
    }
    m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, rendererFlags);
    if (!m_pRenderer)
    {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // The renderer works in CHIP-8 pixels (64x32) and scales to the window by whole
    // multiples only, adding black bars if needed: every CHIP-8 pixel stays a perfect square.
    SDL_RenderSetLogicalSize(m_pRenderer, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    SDL_RenderSetIntegerScale(m_pRenderer, SDL_TRUE);

    // Nearest-neighbour scaling: must be set BEFORE creating the texture, or it is blurred.
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    // STREAMING: the texture is rewritten from the CPU every frame.
    m_pTexture = SDL_CreateTexture(m_pRenderer, SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    if (!m_pTexture)
    {
        std::cerr << "SDL_CreateTexture failed: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

bool CSdlFrontend::processEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            return false;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
            if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
            {
                return false;
            }
            const int chip8Key = mapScancode(event.key.keysym.scancode);
            if (chip8Key >= 0)
            {
                m_aKeys[static_cast<size_t>(chip8Key)] = (event.type == SDL_KEYDOWN);
            }
            break;
        }

        default:
            break;
        }
    }
    return true;
}

void CSdlFrontend::render(const Display& _display)
{
    for (size_t i = 0; i < _display.size(); ++i)
    {
        m_aPixels[i] = _display[i] ? COLOR_ON : COLOR_OFF;
    }

    // Pitch = bytes per row of the source buffer.
    SDL_UpdateTexture(m_pTexture, nullptr, m_aPixels.data(),
        DISPLAY_WIDTH * static_cast<int>(sizeof(uint32_t)));

    SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255); // letterbox bars
    SDL_RenderClear(m_pRenderer);
    SDL_RenderCopy(m_pRenderer, m_pTexture, nullptr, nullptr);
    SDL_RenderPresent(m_pRenderer); // blocks until the next refresh when vsync is on
}

int CSdlFrontend::mapScancode(int _iScancode)
{
    for (const KeyBinding& binding : KEY_BINDINGS)
    {
        if (binding.scancode == _iScancode)
        {
            return binding.chip8Key;
        }
    }
    return -1;
}
