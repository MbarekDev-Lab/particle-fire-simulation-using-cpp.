// ============================================================================
// Screen.cpp — RAII SDL wrapper implementation
//
// Author      : M'Barek Benraiss
// Description : Window, pixel buffer, box blur, and event processing
//
// Changes from original course code:
//   • std::vector replaces new[]/delete[] — automatic, leak-free memory.
//   • std::swap replaces manual pointer swap — exception-safe, self-documenting.
//   • Move constructor/assignment added — lets Screen be returned from functions.
//   • destroy() helper centralizes cleanup (DRY principle).
//   • Dynamic width/height instead of compile-time constants — enables
//     small-buffer headless testing without recompiling.
//   • SDL_GetError() messages on every failure path.
// ============================================================================

#include "Screen.h"

#include <algorithm>
#include <iostream>
#include <utility> // std::swap, std::exchange

namespace particle_sim {

// ---------------------------------------------------------------------------
// Construction / destruction
// ---------------------------------------------------------------------------

Screen::Screen(int width, int height)
    : m_width(width), m_height(height), m_window(nullptr), m_renderer(nullptr), m_texture(nullptr) {
}

Screen::~Screen() {
    destroy();
}

// Move constructor — steal resources from `other`, leave it in a safe state
Screen::Screen(Screen&& other) noexcept
    : m_width(other.m_width), m_height(other.m_height),
      m_window(std::exchange(other.m_window, nullptr)),
      m_renderer(std::exchange(other.m_renderer, nullptr)),
      m_texture(std::exchange(other.m_texture, nullptr)), m_buffer1(std::move(other.m_buffer1)),
      m_buffer2(std::move(other.m_buffer2)) {}

// Move assignment — swap-and-destroy idiom
Screen& Screen::operator=(Screen&& other) noexcept {
    if (this != &other) {
        destroy(); // release our current resources

        m_width = other.m_width;
        m_height = other.m_height;
        m_window = std::exchange(other.m_window, nullptr);
        m_renderer = std::exchange(other.m_renderer, nullptr);
        m_texture = std::exchange(other.m_texture, nullptr);
        m_buffer1 = std::move(other.m_buffer1);
        m_buffer2 = std::move(other.m_buffer2);
    }
    return *this;
}

void Screen::destroy() noexcept {
    // Release SDL resources in reverse creation order.
    // std::vector buffers are freed automatically by their destructor.
    if (m_texture) {
        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
    }
    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
        // Only quit SDL if we owned a window (avoids double-quit on move)
        SDL_Quit();
    }
}

// ---------------------------------------------------------------------------
// Initialization
// ---------------------------------------------------------------------------

bool Screen::init() {
    // Step 1: Initialize the SDL video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Step 2: Create a window
    m_window = SDL_CreateWindow("Particle Fire Simulation", SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED, m_width, m_height, SDL_WINDOW_SHOWN);
    if (!m_window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    // Step 3: Create a VSync renderer (prevents screen tearing)
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (!m_renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
        SDL_Quit();
        return false;
    }

    // Step 4: Create a texture for direct pixel writes (RGBA8888)
    m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC,
                                  m_width, m_height);
    if (!m_texture) {
        std::cerr << "Texture creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
        SDL_Quit();
        return false;
    }

    // Step 5: Allocate pixel buffers (initialized to 0 = black)
    const auto totalPixels = static_cast<std::size_t>(m_width * m_height);
    m_buffer1.assign(totalPixels, 0);
    m_buffer2.assign(totalPixels, 0);

    return true;
}

// ---------------------------------------------------------------------------
// Pixel operations
// ---------------------------------------------------------------------------

void Screen::setPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue) {
    // Bounds check — silently ignore out-of-range coordinates
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return;
    }

    // Pack RGBA into a 32-bit integer (RGBA8888 layout):
    //   Bits 31-24 : Red
    //   Bits 23-16 : Green
    //   Bits 15-8  : Blue
    //   Bits  7-0  : Alpha (0xFF = fully opaque)
    const uint32_t color = (static_cast<uint32_t>(red) << 24) |
                           (static_cast<uint32_t>(green) << 16) |
                           (static_cast<uint32_t>(blue) << 8) | 0xFFu;

    m_buffer1[static_cast<std::size_t>(y * m_width + x)] = color;
}

void Screen::clear() {
    std::fill(m_buffer1.begin(), m_buffer1.end(), 0u);
    std::fill(m_buffer2.begin(), m_buffer2.end(), 0u);
}

// ---------------------------------------------------------------------------
// Box blur — 3×3 convolution kernel producing the glowing trail effect
// ---------------------------------------------------------------------------

void Screen::boxBlur() {
    // Swap buffers: source data moves to m_buffer2,
    // and we write the blurred output into m_buffer1.
    // std::swap is efficient — it swaps internal pointers in O(1).
    std::swap(m_buffer1, m_buffer2);

    // For each pixel, average its color with its 8 neighbors:
    //
    //   [NW] [N] [NE]
    //   [W]  [C] [E]      →  result = sum / 9
    //   [SW] [S] [SE]
    //
    // This creates a soft glow / trail behind moving particles.
    // Integer arithmetic prevents floating-point overhead.
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            int redTotal = 0;
            int greenTotal = 0;
            int blueTotal = 0;

            // Sample the 3×3 neighborhood
            for (int row = -1; row <= 1; ++row) {
                for (int col = -1; col <= 1; ++col) {
                    int sx = x + col;
                    int sy = y + row;

                    // Skip out-of-bounds neighbors (edge pixels get fewer samples,
                    // but dividing by 9 still works — it just darkens edges slightly,
                    // which is the desired fade-to-black behavior).
                    if (sx < 0 || sx >= m_width || sy < 0 || sy >= m_height) {
                        continue;
                    }

                    // Extract RGB from packed 32-bit RGBA color
                    uint32_t color = m_buffer2[static_cast<std::size_t>(sy * m_width + sx)];
                    redTotal += static_cast<uint8_t>(color >> 24);
                    greenTotal += static_cast<uint8_t>(color >> 16);
                    blueTotal += static_cast<uint8_t>(color >> 8);
                }
            }

            // Divide by 9 (integer division — slight darkening is intentional,
            // it makes particles fade to black over time)
            setPixel(x, y, static_cast<uint8_t>(redTotal / 9), static_cast<uint8_t>(greenTotal / 9),
                     static_cast<uint8_t>(blueTotal / 9));
        }
    }
}

// ---------------------------------------------------------------------------
// Rendering
// ---------------------------------------------------------------------------

void Screen::update() {
    SDL_UpdateTexture(m_texture, nullptr, m_buffer1.data(),
                      m_width * static_cast<int>(sizeof(uint32_t)));
    SDL_RenderClear(m_renderer);
    SDL_RenderCopy(m_renderer, m_texture, nullptr, nullptr);
    SDL_RenderPresent(m_renderer);
}

bool Screen::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return false;
        }
        // Allow ESC key to close the window
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            return false;
        }
    }
    return true;
}

} // namespace particle_sim
