// ============================================================================
// Particle.h — Practice class: single particle with polar velocity
//
// Author : M'Barek Benraiss
// ============================================================================

#pragma once

namespace particle_example {

class Particle {
  public:
    Particle();

    /// Randomize direction and speed (squared distribution).
    void initRandom();

    /// Reset position to origin and re-randomize velocity.
    void respawn();

    /// Move the particle by `interval` milliseconds.
    void update(int interval);

    double x;
    double y;
    double speed;
    double direction;
};

} // namespace particle_example
