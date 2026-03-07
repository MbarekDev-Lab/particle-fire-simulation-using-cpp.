// ============================================================================
// Particle.h — Single particle with polar velocity
//
// Author      : M'Barek Benraiss
// Description : Particle struct with position, speed, direction, update logic
//
// Design decisions:
//   • struct (not class) — all fields are public because the Swarm and main
//     loop both need direct access for rendering; encapsulation would add
//     getters without benefit here.
//   • Polar velocity (speed + direction) rather than Cartesian (vx, vy) —
//     makes explosion effects trivial: randomize direction, done.
//   • No virtual destructor — Particle is a value type, never used
//     polymorphically. Avoids vtable overhead for 5000+ instances.
//   • initRandom() separated from constructor so particles can be
//     re-randomized (respawn) without constructing a new object.
//
// Key Concepts:
//   • Polar ↔ Cartesian conversion via cos/sin
//   • Squared speed distribution for natural clustering
// ============================================================================

#pragma once

namespace particle_sim {

struct Particle {
    // Position in normalized space: x ∈ [-1, 1], y ∈ [-1, 1]
    // This makes the simulation resolution-independent.
    double x;
    double y;

    // Velocity in polar form — angle in radians, speed in units/ms
    double speed;
    double direction;

    Particle();

    /// Randomize direction and speed (called on construction and respawn).
    void initRandom();

    /// Reset position to origin and re-randomize velocity.
    /// Called when a particle leaves the visible area.
    void respawn();

    /// Advance position by `interval` milliseconds using polar→Cartesian math.
    /// Automatically respawns if the particle goes off-screen.
    void update(int interval);
};

} // namespace particle_sim
