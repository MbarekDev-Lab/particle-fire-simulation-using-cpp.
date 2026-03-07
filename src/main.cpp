// ============================================================================
// main.cpp — Particle Fire Simulation entry point
//
// Author      : M'Barek Benraiss
// Description : Main loop tying Screen and Swarm together
//
// This file is intentionally small. It:
//   1. Seeds the RNG
//   2. Initializes the Screen (SDL window + pixel buffers)
//   3. Creates a Swarm of particles
//   4. Runs the update → draw → blur → display loop
//   5. Exits when the user closes the window (Screen destructor cleans up)
//
// Color cycling uses sine waves with different frequencies so the particles
// smoothly shift through the RGB spectrum over time.
//
// Controls:
//   Close window → quit
// ============================================================================

#include "Screen.h"
#include "Swarm.h"

#include <SDL.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>

using particle_sim::Screen;
using particle_sim::Swarm;

/// Map a sine wave to the 0–255 color range.
/// Different frequencies for R, G, B create smooth color cycling.
static uint8_t colorChannel(double frequency, int elapsedMs) {
    return static_cast<uint8_t>((1.0 + std::sin(elapsedMs * frequency)) * 128);
}

int main() {
    // Seed RNG once (used by Particle::initRandom for direction/speed)
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // --- Initialize Screen ---
    Screen screen;
    if (!screen.init()) {
        std::cerr << "Error initializing SDL." << std::endl;
        return 1;
    }

    // --- Create Particle Swarm ---
    Swarm swarm; // default: 5000 particles

    // --- Main loop: update → draw → blur → display ---
    while (true) {
        int elapsed = SDL_GetTicks();

        // Advance all particles (frame-rate independent via delta time)
        swarm.update(elapsed);

        // Cycle colors using sine waves at different frequencies
        uint8_t red = colorChannel(0.0002, elapsed);
        uint8_t green = colorChannel(0.0001, elapsed);
        uint8_t blue = colorChannel(0.0003, elapsed);

        // Draw each particle: map normalized coords [-1,1] → screen pixels
        for (const auto& p : swarm.getParticles()) {
            int sx = static_cast<int>((p.x + 1.0) * screen.width() / 2);
            int sy = static_cast<int>(p.y * screen.width() / 2 + screen.height() / 2);
            screen.setPixel(sx, sy, red, green, blue);
        }

        // Blur creates the glowing fire trail effect
        screen.boxBlur();

        // Push pixel buffer to the display
        screen.update();

        // Exit on window close
        if (!screen.processEvents()) {
            break;
        }
    }

    // Screen destructor releases all SDL resources (RAII)
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
