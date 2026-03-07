// ============================================================================
// Swarm.h — Manages a collection (swarm) of particles
//
// Author      : M'Barek Benraiss
// Description : Owns particles, handles bulk updates with delta-time
//
// Design decisions:
//   • Configurable particle count via constructor (default 5000).
//     This lets tests use a small count without recompiling.
//   • std::vector<Particle> for storage — contiguous memory, cache-friendly.
//   • Read-only access via const reference — rendering code can iterate
//     without being able to modify particle state.
//   • Delta-time tracking: stores last elapsed time and computes interval
//     to decouple simulation speed from frame rate.
// ============================================================================

#pragma once

#include "Particle.h"

#include <vector>

namespace particle_sim {

class Swarm {
  public:
    static constexpr int DEFAULT_NUM_PARTICLES = 5000;

    /// Construct a swarm with `count` particles (all start at origin).
    explicit Swarm(int count = DEFAULT_NUM_PARTICLES);

    /// Advance all particles by the delta since last call.
    /// @param elapsedMs  Milliseconds since program start (e.g. SDL_GetTicks()).
    void update(int elapsedMs);

    /// Read-only access to the particle array (for rendering).
    const std::vector<Particle>& getParticles() const { return m_particles; }

    /// Number of particles in the swarm.
    int count() const { return static_cast<int>(m_particles.size()); }

  private:
    std::vector<Particle> m_particles;
    int m_lastUpdateTime;
};

} // namespace particle_sim
