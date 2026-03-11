// ============================================================================
// Particle.cpp — Practice implementation: polar-velocity particle
//
// Author : M'Barek Benraiss
//
// Enhancements over original:
//   • initRandom() and respawn() separated for clarity
//   • Portable PI constant (M_PI is POSIX, not standard C++)
//   • Squared speed distribution: (rand/RAND_MAX)² clusters most particles
//     at low speeds — looks more natural
//   • Off-screen check → respawn() keeps the explosion looping
//   • C++ headers (<cmath>, <cstdlib>) replace C versions
// ============================================================================

#include "Particle.h"

#include <cmath>
#include <cstdlib>

namespace particle_example {

// Portable π — M_PI is a POSIX extension, not guaranteed in C++17.
static constexpr double PI = 3.14159265358979323846;

Particle::Particle() : x(0), y(0), speed(0), direction(0) {
    initRandom();
}

void Particle::initRandom() {
    // Random direction: 0 to 2π (full circle — all angles equally likely)
    direction = (2.0 * PI * std::rand()) / RAND_MAX;

    // Squared speed distribution:
    // normalizedRand ∈ [0, 1], squaring clusters values near 0.
    // Result: many slow particles, few fast ones — visually natural.
    double r = static_cast<double>(std::rand()) / RAND_MAX;
    speed = r * r * 0.04; // max ≈ 0.04 units/ms
}

void Particle::respawn() {
    x = 0;
    y = 0;
    initRandom();
}

void Particle::update(int interval) {
    // Convert polar velocity to Cartesian displacement:
    //   dx = speed × cos(direction) × time
    //   dy = speed × sin(direction) × time
    x += speed * std::cos(direction) * interval;
    y += speed * std::sin(direction) * interval;

    // Respawn particles that leave the visible area [-1, 1]
    if (x < -1.0 || x > 1.0 || y < -1.0 || y > 1.0) {
        respawn();
    }
}

} // namespace particle_example
