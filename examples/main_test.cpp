//
// Created by M'Barek Benraiss on 08.03.26.
//
// Practice: particle fire simulation with color cycling
//
// Part 1: Bit-packing exercise (RGBA → uint32)
// Part 2: Particle simulation with sine-wave color cycling
//

#include "Screen.h"
#include "Swarm.h"

#include <SDL.h>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>

using particle_sim::Screen;
using particle_sim::Swarm;
using std::cout;
using std::dec;
using std::endl;
using std::hex;
using std::setfill;
using std::setw;

int main() {

    // ── Part 1: Bit-packing exercise ─────────────────────────────────────
    //
    // Pack four 8-bit channels (ARGB) into a single 32-bit integer.
    // Target result: 0xFF123456
    //
    //  Byte layout in a uint32_t:
    //  ┌────────┬────────┬────────┬────────┐
    //  │ Alpha  │  Red   │ Green  │  Blue  │
    //  │ bits   │ bits   │ bits   │ bits   │
    //  │ 31-24  │ 23-16  │ 15-8   │  7-0   │
    //  └────────┴────────┴────────┴────────┘

    unsigned char alpha = 0xFF;
    unsigned char red = 0x12;
    unsigned char green = 0x34;
    unsigned char blue = 0x56;

    // Method 1: Bitwise OR — cast to uint32_t first to avoid UB.
    // Without the cast, unsigned char promotes to *signed* int,
    // and (0xFF << 24) overflows a 32-bit signed int → undefined behavior.
    uint32_t color = (static_cast<uint32_t>(alpha) << 24) | (static_cast<uint32_t>(red) << 16) |
                     (static_cast<uint32_t>(green) << 8) | static_cast<uint32_t>(blue);

    // Method 2: Shift-and-add — build from most significant byte down.
    // No UB here because color2 is unsigned, so all arithmetic stays unsigned.
    uint32_t color2 = 0;
    color2 += alpha;
    color2 <<= 8;
    color2 += red;
    color2 <<= 8;
    color2 += green;
    color2 <<= 8;
    color2 += blue;

    cout << "Method 1: " << setfill('0') << setw(8) << hex << color << endl;
    cout << "Method 2: " << setfill('0') << setw(8) << hex << color2 << endl;
    cout << dec; // Reset to decimal — avoids accidental hex output later

    // ── Part 2: Particle simulation ──────────────────────────────────────

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    Screen screen;
    if (!screen.init()) {
        cout << "Error initialising SDL." << endl;
        return 1;
    }

    Swarm swarm;

    while (true) {
        // SDL_GetTicks() returns Uint32 — store as uint32_t to avoid narrowing
        uint32_t elapsed = SDL_GetTicks();

        // Update particles (frame-rate independent via delta time)
        swarm.update(static_cast<int>(elapsed));

        // Sine-wave color cycling at different frequencies
        auto r = static_cast<uint8_t>((1.0 + std::sin(elapsed * 0.0002)) * 128);
        auto g = static_cast<uint8_t>((1.0 + std::sin(elapsed * 0.0001)) * 128);
        auto b = static_cast<uint8_t>((1.0 + std::sin(elapsed * 0.0003)) * 128);

        // Draw each particle: map normalized coords [-1,1] → screen pixels
        for (const auto& p : swarm.getParticles()) {
            int sx = static_cast<int>((p.x + 1.0) * screen.width() / 2);
            int sy = static_cast<int>(p.y * screen.width() / 2 + screen.height() / 2);
            screen.setPixel(sx, sy, r, g, b);
        }

        // boxBlur() fades old pixels → creates glowing fire trail.
        // No screen.clear() needed: the blur IS the fade.
        screen.boxBlur();

        screen.update();

        if (!screen.processEvents()) {
            break;
        }
    }

    return 0;
}
/*
from Makefile:
cd build && make main-test && ./main-test

from the project root :
cmake --build build --target main-test && ./build/main-test
*/