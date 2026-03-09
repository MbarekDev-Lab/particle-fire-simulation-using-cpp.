//
// Created by M'Barek Benraiss on 08.03.26.
//

#include "Screen.h"

#include <SDL.h>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "Swarm.h"

using particle_sim::Screen;
using particle_sim::Swarm;
using std::cout;
using std::endl;
using std::hex;
using std::setfill;
using std::setw;

int main() {

    // 0xFF123456

    unsigned char alpha = 0xFF;
    unsigned char red = 0x12;
    unsigned char green = 0x34;
    unsigned char blue = 0x56;

    unsigned int color = 0;

    color |= alpha << 24;
    color |= red << 16;
    color |= green << 8;
    color |= blue;

    unsigned int color2 = 0;

    color2 += alpha;
    color2 <<= 8;

    color2 += red;
    color2 <<= 8;

    color2 += green;
    color2 <<= 8;

    color2 += blue;

    cout << hex << color << endl;
    cout << setfill('0') << setw(8) << hex << color2 << endl; // FF123456

    /*
    | Alpha | Red | Green | Blue |
    |  8bit | 8bit|  8bit | 8bit |
     */

    srand(static_cast<unsigned int>(time(nullptr)));

    Screen screen;

    if (!screen.init()) {
        cout << "Error initialising SDL." << endl;
        return 1;
    }

    Swarm swarm;
    const auto& particles = swarm.getParticles();


    while (true) {
        int elapsed = SDL_GetTicks();

        auto green = static_cast<unsigned char>((1 + std::sin(elapsed * 0.0001)) * 128);
        auto red = static_cast<unsigned char>((1 + std::sin(elapsed * 0.0002)) * 128);
        auto blue = static_cast<unsigned char>((1 + std::sin(elapsed * 0.0003)) * 128);

        for (int y = 0; y < screen.height(); y++) {
            for (int x = 0; x < screen.width(); x++) {
                screen.setPixel(x, y, red, green, blue);
            }
        }

        screen.update();

        if (!screen.processEvents()) {
            break;
        }
    }

    return 0;
}

/*
  cd build && make practice && ./practice

*/