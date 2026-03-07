// ============================================================================
// Screen.h — RAII SDL wrapper: window, renderer, texture, and pixel buffers
//
// Author      : M'Barek Benraiss
// Description : Encapsulates all SDL graphics resources behind a safe API
//
// Design decisions:
//   • RAII — constructor acquires nothing; init() creates SDL objects;
//     destructor releases everything. No separate close() needed.
//   • Copy is deleted because SDL handles are unique OS resources.
//   • Move is provided so Screen can be returned from factory functions.
//   • Pixel buffers use std::vector<uint32_t> — no manual new/delete.
//   • Two buffers enable the box-blur effect without read/write aliasing.
//   • Member naming: m_ prefix for private data members (project convention).
//   • Width/height are template-style constexpr so they can be overridden
//     at construction time for headless testing without recompiling.
//
// Key Concepts:
//   • RGBA8888 pixel packing via bit shifts
//   • 3×3 box blur (convolution kernel) for glow trails
//   • Double buffering for artifact-free blur
// ============================================================================

#pragma once

#include <SDL.h>
#include <cstdint>
#include <string>
#include <vector>

namespace particle_sim {

class Screen {
  public:
    // Default dimensions — used by the main simulation
    static constexpr int DEFAULT_WIDTH = 800;
    static constexpr int DEFAULT_HEIGHT = 600;

    /// Construct with custom dimensions (useful for headless tests).
    explicit Screen(int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT);
    ~Screen();

    // --- Rule of Five: copy deleted, move implemented ---
    Screen(const Screen&) = delete;
    Screen& operator=(const Screen&) = delete;
    Screen(Screen&& other) noexcept;
    Screen& operator=(Screen&& other) noexcept;

    /// Initialize SDL subsystem, window, renderer, texture, and pixel buffers.
    /// @return false on failure (diagnostic printed to stderr).
    bool init();

    /// Write a single pixel. Out-of-bounds coordinates are silently ignored.
    void setPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue);

    /// Clear both pixel buffers to black.
    void clear();

    /// Apply a 3×3 box blur (averages each pixel with its 8 neighbors).
    /// Safe for future multithreading: reads from buffer2, writes to buffer1,
    /// with no shared mutable state beyond the two vectors.
    void boxBlur();

    /// Push the current pixel buffer to screen (texture upload + render).
    void update();

    /// Poll SDL events. Returns false when the user requests quit.
    bool processEvents();

    // --- Read-only accessors (useful for tests and coordinate mapping) ---
    int width() const { return m_width; }
    int height() const { return m_height; }

    /// Direct read-only access to the front pixel buffer (for test assertions).
    const std::vector<uint32_t>& pixelBuffer() const { return m_buffer1; }

  private:
    void destroy() noexcept; // Release SDL resources

    int m_width;
    int m_height;

    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    SDL_Texture* m_texture;

    std::vector<uint32_t> m_buffer1; // Front buffer (written to / displayed)
    std::vector<uint32_t> m_buffer2; // Back buffer  (source during blur)
};

} // namespace particle_sim
