// ============================================================================
// Swarm.h — Practice class: manages a collection of particles
//
// Author : M'Barek Benraiss
//
// Enhancements over original:
//   • Replaced raw pointer + manual new/delete with std::vector (RAII)
//   • Default destructor — vector cleans up automatically
//   • count() accessor for particle count
//   • Delta-time tracking via m_lastUpdateTime
// ============================================================================

#pragma once

#include "Particle.h"

#include <vector>

namespace particle_example {

class Swarm {
  public:
    static constexpr int NPARTICLES = 5000;

    Swarm();
    ~Swarm() = default;

    /// Advance all particles by the delta since the last call.
    /// @param elapsedMs  Milliseconds since program start (e.g. SDL_GetTicks()).
    void update(int elapsedMs);

    /// Read-only access to the particle array (for rendering).
    const std::vector<Particle>& getParticles() const { return m_particles; }

    /// Number of particles in the swarm.
    int count() const { return static_cast<int>(m_particles.size()); }

  private:
    std::vector<Particle> m_particles;
    int m_lastUpdateTime{0};
};

} // namespace particle_example
