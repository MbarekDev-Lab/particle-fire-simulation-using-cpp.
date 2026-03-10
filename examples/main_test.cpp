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
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>

using particle_sim::Screen;
using particle_sim::Swarm;
using std::cout;
using std::endl;
using std::hex;
using std::setfill;
using std::setw;

int main() {

    // ── Part 1: Bit-packing exercise ─────────────────────────────────────

    // Target: 0xFF123456
    unsigned char alpha = 0xFF;
    unsigned char red = 0x12;
    unsigned char green = 0x34;
    unsigned char blue = 0x56;

    // Method 1: Bitwise OR — shift each channel into position
    unsigned int color = 0;
    color |= alpha << 24;
    color |= red << 16;
    color |= green << 8;
    color |= blue;

    // Method 2: Shift-and-add — build from most significant byte down
    unsigned int color2 = 0;
    color2 += alpha;
    color2 <<= 8;
    color2 += red;
    color2 <<= 8;
    color2 += green;
    color2 <<= 8;
    color2 += blue;

    cout << hex << color << endl;
    cout << setfill('0') << setw(8) << hex << color2 << endl;

    /*
    | Alpha | Red | Green | Blue |
    |  8bit | 8bit|  8bit | 8bit |
     */

    // ── Part 2: Particle simulation ──────────────────────────────────────

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    Screen screen;
    if (!screen.init()) {
        cout << "Error initialising SDL." << endl;
        return 1;
    }

    Swarm swarm;

    while (true) {
        int elapsed = SDL_GetTicks();

        // Update particles (frame-rate independent via delta time)
        swarm.update(elapsed);

        // Sine-wave color cycling at different frequencies
        auto r = static_cast<unsigned char>((1 + std::sin(elapsed * 0.0002)) * 128);
        auto g = static_cast<unsigned char>((1 + std::sin(elapsed * 0.0001)) * 128);
        auto b = static_cast<unsigned char>((1 + std::sin(elapsed * 0.0003)) * 128);

        // Draw each particle: map normalized coords [-1,1] → screen pixels
        for (const auto& p : swarm.getParticles()) {
            int sx = static_cast<int>((p.x + 1.0) * screen.width() / 2);
            int sy = static_cast<int>(p.y * screen.width() / 2 + screen.height() / 2);
            screen.setPixel(sx, sy, r, g, b);
        }

        // Blur creates the glowing fire trail effect
        screen.boxBlur();

        // Push pixel buffer to the display
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