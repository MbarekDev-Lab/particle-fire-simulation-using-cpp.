// ============================================================================
// smoke_test.cpp — Headless smoke test for Screen pixel buffer and box blur
//
// Author      : M'Barek Benraiss
// Description : Validates core rendering logic without displaying a window
//
// This test:
//   1. Creates a small Screen (64×64) — SDL can create a hidden window.
//   2. Draws a known pattern (white cross in center) into the pixel buffer.
//   3. Verifies the pattern was written correctly.
//   4. Runs boxBlur() and verifies it changed the buffer (non-trivial output).
//   5. Writes the result to out/frame0.ppm (PPM format — no external libs).
//   6. Tests that Screen destructor cleans up without crash (RAII check).
//   7. Verifies Particle and Swarm basic behavior.
//
// Exit code: 0 = all checks passed, 1 = failure.
//
// Build: cmake --build build --target smoke-test
// Run:   ./build/smoke-test
// ============================================================================

#include "Particle.h"
#include "Screen.h"
#include "Swarm.h"

#include <SDL.h>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Number of assertions passed
static int g_passed = 0;

#define CHECK(cond, msg)                                                                           \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            std::cerr << "FAIL: " << (msg) << " (" << __FILE__ << ":" << __LINE__ << ")"           \
                      << std::endl;                                                                \
            return 1;                                                                              \
        }                                                                                          \
        ++g_passed;                                                                                \
    } while (0)

/// Write a pixel buffer to a PPM (Portable Pixel Map) file.
/// PPM is a simple uncompressed image format that needs no external libraries.
static bool writePPM(const std::string& path, const std::vector<uint32_t>& buffer, int width,
                     int height) {
    std::ofstream out(path, std::ios::binary);
    if (!out)
        return false;

    // PPM header: P6 = binary RGB, then width height maxval
    out << "P6\n" << width << " " << height << "\n255\n";

    for (int i = 0; i < width * height; ++i) {
        uint32_t color = buffer[static_cast<std::size_t>(i)];
        // RGBA8888: extract R, G, B (ignore alpha)
        uint8_t r = static_cast<uint8_t>(color >> 24);
        uint8_t g = static_cast<uint8_t>(color >> 16);
        uint8_t b = static_cast<uint8_t>(color >> 8);
        out.put(static_cast<char>(r));
        out.put(static_cast<char>(g));
        out.put(static_cast<char>(b));
    }

    return out.good();
}

/// Count non-zero pixels in the buffer.
static int countNonZero(const std::vector<uint32_t>& buf) {
    int count = 0;
    for (auto px : buf) {
        if (px != 0)
            ++count;
    }
    return count;
}

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    constexpr int W = 64;
    constexpr int H = 64;

    std::cout << "=== Particle Fire Smoke Test ===" << std::endl;

    // -----------------------------------------------------------------------
    // Test 1: Screen creation and init
    // -----------------------------------------------------------------------
    std::cout << "Test 1: Screen init (64x64)... ";
    {
        particle_sim::Screen screen(W, H);
        bool ok = screen.init();
        CHECK(ok, "Screen::init() should succeed");
        CHECK(screen.width() == W, "Width should be 64");
        CHECK(screen.height() == H, "Height should be 64");
        std::cout << "OK" << std::endl;
    }
    // Screen destroyed here — RAII test: no crash = pass
    std::cout << "  (RAII destructor: OK)" << std::endl;

    // -----------------------------------------------------------------------
    // Test 2: setPixel and pixel buffer
    // -----------------------------------------------------------------------
    std::cout << "Test 2: setPixel + buffer read... ";
    particle_sim::Screen screen(W, H);
    CHECK(screen.init(), "Screen::init()");

    // Draw a white cross in the center
    for (int i = 0; i < W; ++i) {
        screen.setPixel(i, H / 2, 255, 255, 255); // horizontal line
        screen.setPixel(W / 2, i, 255, 255, 255); // vertical line
    }

    const auto& buf = screen.pixelBuffer();
    CHECK(buf.size() == static_cast<std::size_t>(W * H), "Buffer size");

    // Center pixel (intersection of cross) should be non-zero
    uint32_t centerPixel = buf[static_cast<std::size_t>(H / 2 * W + W / 2)];
    CHECK(centerPixel != 0, "Center pixel should be non-zero after setPixel");

    int nonZeroBefore = countNonZero(buf);
    CHECK(nonZeroBefore > 0, "Should have non-zero pixels after drawing");
    std::cout << "OK (" << nonZeroBefore << " lit pixels)" << std::endl;

    // -----------------------------------------------------------------------
    // Test 3: out-of-bounds setPixel (should not crash)
    // -----------------------------------------------------------------------
    std::cout << "Test 3: Out-of-bounds setPixel... ";
    screen.setPixel(-1, -1, 255, 0, 0);
    screen.setPixel(W, H, 255, 0, 0);
    screen.setPixel(9999, 9999, 255, 0, 0);
    std::cout << "OK (no crash)" << std::endl;
    ++g_passed;

    // -----------------------------------------------------------------------
    // Test 4: boxBlur modifies the buffer
    // -----------------------------------------------------------------------
    std::cout << "Test 4: boxBlur changes pixel buffer... ";

    // Save center pixel value before blur
    (void)screen.pixelBuffer()[static_cast<std::size_t>(H / 2 * W + W / 2)];

    screen.boxBlur();

    const auto& bufAfter = screen.pixelBuffer();
    int nonZeroAfter = countNonZero(bufAfter);

    // After blur, more pixels should be non-zero (blur spreads the cross)
    CHECK(nonZeroAfter > nonZeroBefore, "Blur should spread pixels (more non-zero after)");

    // Center pixel should still be non-zero (it was surrounded by lit pixels)
    uint32_t afterBlur = bufAfter[static_cast<std::size_t>(H / 2 * W + W / 2)];
    CHECK(afterBlur != 0, "Center pixel should survive blur");

    std::cout << "OK (" << nonZeroBefore << " → " << nonZeroAfter << " lit pixels)" << std::endl;

    // -----------------------------------------------------------------------
    // Test 5: Write PPM output
    // -----------------------------------------------------------------------
    std::cout << "Test 5: Write out/frame0.ppm... ";
    CHECK(writePPM("out/frame0.ppm", screen.pixelBuffer(), W, H), "PPM file write should succeed");
    std::cout << "OK" << std::endl;

    // -----------------------------------------------------------------------
    // Test 6: Particle basic behavior
    // -----------------------------------------------------------------------
    std::cout << "Test 6: Particle initRandom + update + respawn... ";
    {
        particle_sim::Particle p;
        CHECK(p.x == 0.0 && p.y == 0.0, "Particle starts at origin");

        // After a small update, particle should have moved
        p.update(1); // 1 ms — small enough to stay on screen
        bool moved = (p.x != 0.0 || p.y != 0.0);
        CHECK(moved, "Particle should move after update");

        // Respawn should reset to origin
        p.respawn();
        CHECK(p.x == 0.0 && p.y == 0.0, "Particle should be at origin after respawn");
    }
    std::cout << "OK" << std::endl;

    // -----------------------------------------------------------------------
    // Test 7: Swarm basic behavior
    // -----------------------------------------------------------------------
    std::cout << "Test 7: Swarm creation and update... ";
    {
        particle_sim::Swarm swarm(100); // small swarm for testing
        CHECK(swarm.count() == 100, "Swarm should have 100 particles");

        swarm.update(16); // simulate 16ms
        const auto& particles = swarm.getParticles();
        bool anyMoved = false;
        for (const auto& p : particles) {
            if (p.x != 0.0 || p.y != 0.0) {
                anyMoved = true;
                break;
            }
        }
        CHECK(anyMoved, "At least one particle should have moved");
    }
    std::cout << "OK" << std::endl;

    // -----------------------------------------------------------------------
    // Test 8: clear() resets buffer
    // -----------------------------------------------------------------------
    std::cout << "Test 8: clear() resets buffer... ";
    screen.clear();
    CHECK(countNonZero(screen.pixelBuffer()) == 0, "Buffer should be all zeros after clear");
    std::cout << "OK" << std::endl;

    // -----------------------------------------------------------------------
    // Summary
    // -----------------------------------------------------------------------
    std::cout << "\n=== ALL " << g_passed << " CHECKS PASSED ===" << std::endl;
    return 0;
}


/*
benraiss@MacBookAir particle-fire-simulation-using-cpp % mkdir -p build && cd build
cmake ..
make

# Run the simulation
./particle-fire

# Run the SDL basics example
./sdl-basics

# Run the smoke test
mkdir -p out && ./smoke-test
-- SDL2 include:
/opt/homebrew/Cellar/sdl2/2.32.10/include;/opt/homebrew/Cellar/sdl2/2.32.10/include/SDL2
-- SDL2 libs:    SDL2::SDL2
-- C++ standard: 17
-- Configuring done (0.1s)
-- Generating done (0.0s)
-- Build files have been written to:
/Users/benraiss/Documents/particle-fire-simulation-using-cpp/build [ 40%] Built target particle-core
[ 60%] Built target particle-fire
[ 80%] Built target sdl-basics
[100%] Built target smoke-test
zsh: command not found: #
zsh: command not found: #
zsh: command not found: #
=== Particle Fire Smoke Test ===
Test 1: Screen init (64x64)... OK
  (RAII destructor: OK)
Test 2: setPixel + buffer read... OK (127 lit pixels)
Test 3: Out-of-bounds setPixel... OK (no crash)
Test 4: boxBlur changes pixel buffer... OK (127 → 4096 lit pixels)
Test 5: Write out/frame0.ppm... OK
Test 6: Particle initRandom + update + respawn... OK
Test 7: Swarm creation and update... OK
Test 8: clear() resets buffer... OK

=== ALL 17 CHECKS PASSED ===
benraiss@MacBookAir build %

or using the helper script:

./tools/run_smoke_test.sh

*/
