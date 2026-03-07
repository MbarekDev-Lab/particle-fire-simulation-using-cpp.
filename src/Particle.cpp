// ============================================================================
// Particle.cpp — Particle movement and initialization
//
// Author      : M'Barek Benraiss
// Description : Polar-velocity particle with squared speed distribution
//
// Changes from original course code:
//   • init() renamed to initRandom() for clarity.
//   • respawn() added — resets position AND re-randomizes velocity.
//   • Squared speed distribution: (rand/RAND_MAX)² clusters most particles
//     at low speeds with a few fast outliers — looks more natural.
//   • Off-screen check uses respawn() — keeps the explosion looping forever.
//   • C++ headers (<cmath>, <cstdlib>) replace C versions.
//
// Coordinate mapping (direction → motion):
//   0      → right   (cos=1,  sin=0)
//   π/2    → down    (cos=0,  sin=1)
//   π      → left    (cos=-1, sin=0)
//   3π/2   → up      (cos=0,  sin=-1)
// ============================================================================

#include "Particle.h"

#include <cmath>
#include <cstdlib>

namespace particle_sim {

// Portable π constant — M_PI is a POSIX extension, not standard C++.
static constexpr double PI = 3.14159265358979323846;

Particle::Particle() : x(0), y(0), speed(0), direction(0) {
    initRandom();
}

void Particle::initRandom() {
    // Random direction: 0 to 2π (full circle — all angles equally likely)
    direction = (2.0 * PI * rand()) / RAND_MAX;

    // Squared speed distribution:
    // normalizedRand ∈ [0, 1], squaring it clusters values near 0.
    // Result: many slow particles, few fast ones — visually natural.
    double r = static_cast<double>(rand()) / RAND_MAX;
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

} // namespace particle_sim
